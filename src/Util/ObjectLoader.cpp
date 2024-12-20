#include <SimView/Util/ObjectLoader.hpp>

#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#endif

#include <SimView/Util/llas.hpp>

namespace simview {
namespace util {

std::vector<std::string> ObjectLoader::getReadableExtensionList() {
  std::vector<std::string> extensionList;

  // Native support
  extensionList.push_back("msh");

  // Native support
  extensionList.push_back("pch");

  // Native support
  extensionList.push_back("las");

#if defined(SIMVIEW_WITH_ASSIMP)
  std::string strExtensions;

  {
    Assimp::Importer importer;
    importer.GetExtensionList(strExtensions);
  }

  // Remove '*'
  strExtensions = StringUtil::remove(strExtensions, '*');

  // Separate by ';'
  std::vector<std::string> tmpExtensions = StringUtil::splitText(strExtensions, ';');

  for (std::string extension : tmpExtensions) {
    // Remove '.' at head
    extension.erase(extension.begin());
    extensionList.push_back(extension);
  }
#else

  // Native support
  extensionList.push_back("obj");
#endif

#if defined(SIMVIEW_WITH_VTK)
  extensionList.push_back("vtk");
  extensionList.push_back("vtu");
#endif

  return extensionList;
}

void ObjectLoader::readFromFile(const std::string &filePath,
                                VertexArray_t vertices,
                                IndexArray_t indices,
                                const float offsetX,
                                const float offsetY,
                                const float offsetZ,
                                const bool autoScale) {
  const std::string extension = FileUtil::extension(filePath);

  LOG_INFO("### Start loading object file: " + filePath);

  const auto startTime = std::chrono::system_clock::now();

  {
    if (extension == ".msh") {
      readMshFile(filePath, vertices, indices, offsetX, offsetY, offsetZ);
    } else if (extension == ".pch") {
      readPchFile(filePath, vertices, indices, offsetX, offsetY, offsetZ);
    } else if (extension == ".las") {
      readLasFile(filePath, vertices, indices, offsetX, offsetY, offsetZ);
    } else if (extension == ".vtk" || extension == ".vtu") {
      readVtkFile(filePath, vertices, indices, offsetX, offsetY, offsetZ);
    } else {
      readObjFile(filePath, vertices, indices, offsetX, offsetY, offsetZ);
    }

    if (autoScale) {
      // Automatically adjust model scale to [-1.0, 1.0]
      glm::vec3 maxCoords, minCoords;
      std::tie(minCoords, maxCoords) = ObjectLoader::getCorners(vertices);

      const glm::vec3 modelScale = maxCoords - minCoords;
      const float modelScaleMax = std::max(modelScale.x, std::max(modelScale.y, modelScale.z));

      const float mag = 2.0f / modelScaleMax;
      ObjectLoader::scaleObject(vertices, mag);

      ObjectLoader::moveToOrigin(vertices);
      ObjectLoader::translateObject(vertices, offsetX, offsetY, offsetZ);
    }
  }

  const auto endTime = std::chrono::system_clock::now();
  const double elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

  LOG_INFO("### Done. Elapsed time: " + std::to_string(elapsedTime / 1e6) + " [sec]");
}

void ObjectLoader::readObjFile(const std::string &filePath,
                               VertexArray_t vertices,
                               IndexArray_t indices,
                               const float offsetX,
                               const float offsetY,
                               const float offsetZ) {
#if defined(SIMVIEW_WITH_ASSIMP)
  Assimp::Importer importer;
  unsigned int flag = 0;
  flag |= aiProcess_Triangulate;
  flag |= aiProcess_CalcTangentSpace;
  flag |= aiProcess_RemoveRedundantMaterials;
  flag |= aiProcess_GenNormals;

  const aiScene *scene = importer.ReadFile(filePath, flag);

  if (scene == nullptr) {
    LOG_ERROR("[ERROR] " + std::string(importer.GetErrorString()));
    return;
  }

  for (int iMesh = 0; iMesh < scene->mNumMeshes; ++iMesh) {
    aiMesh *mesh = scene->mMeshes[iMesh];

    for (int iFace = 0; iFace < mesh->mNumFaces; ++iFace) {
      const aiFace &face = mesh->mFaces[iFace];

      for (int iVertex = 0; iVertex < face.mNumIndices; ++iVertex) {
        const unsigned int index = face.mIndices[iVertex];

        glm::vec3 position = glm::vec3(mesh->mVertices[index].x, mesh->mVertices[index].y, mesh->mVertices[index].z);
        glm::vec3 normal(0.0f), color(1.0f);
        glm::vec2 texcoord(0.0f);

        if (mesh->HasNormals()) {
          normal = glm::vec3(mesh->mNormals[index].x, mesh->mNormals[index].y, mesh->mNormals[index].z);
        }

        if (mesh->HasTextureCoords(0)) {
          texcoord = glm::vec2(mesh->mTextureCoords[0][index].x, mesh->mTextureCoords[0][index].y);
        }

        const unsigned int materialID = mesh->mMaterialIndex;

        const Vertex vertex(position,
                            color,
                            normal,
                            BARY_CENTER[iVertex % 3],
                            texcoord,
                            0.0f);

        indices->push_back(uint32_t(vertices->size()));
        vertices->push_back(vertex);
      }
    }
  }
#else
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;

  bool success = tinyobj::LoadObj(
      &attrib,
      &shapes,
      &materials,
      &warn,
      &err,
      filePath.c_str());

  if (!warn.empty()) {
    LOG_WARN("[WARNING] " + warn);
  }

  if (!err.empty()) {
    LOG_ERROR("[Error] " + err);
  }

  if (!success) {
    throw std::runtime_error("Failed to load OBJ file: " + filePath);
  }

  // Create vertex array

  glm::vec3 maxCoords(0.0f);
  glm::vec3 minCoords(0.0f);

  for (int s = 0; s < shapes.size(); ++s) {
    const tinyobj::mesh_t &mesh = shapes[s].mesh;

    int nVertices = mesh.indices.size();

    for (int i = 0; i < nVertices; ++i) {
      const tinyobj::index_t &index = mesh.indices[i];

      glm::vec3 position(0.0f), normal(0.0f), color(0.0f);
      glm::vec2 texcoord(0.0f);

      if (index.vertex_index >= 0) {
        position = glm::vec3(attrib.vertices[index.vertex_index * 3 + 0], attrib.vertices[index.vertex_index * 3 + 1], attrib.vertices[index.vertex_index * 3 + 2]);
        color = glm::vec3(attrib.colors[index.vertex_index * 3 + 0], attrib.colors[index.vertex_index * 3 + 1], attrib.colors[index.vertex_index * 3 + 2]);

        for (int direction = 0; direction < 3; direction++) {
          if (position[direction] > maxCoords[direction]) {
            maxCoords[direction] = position[direction];
          }
          if (position[direction] < minCoords[direction]) {
            minCoords[direction] = position[direction];
          }
        }
      }

      if (index.normal_index >= 0) {
        normal = glm::vec3(attrib.normals[index.normal_index * 3 + 0], attrib.normals[index.normal_index * 3 + 1], attrib.normals[index.normal_index * 3 + 2]);
      }

      if (index.texcoord_index >= 0) {
        texcoord = glm::vec2(attrib.texcoords[index.texcoord_index * 2 + 0], attrib.texcoords[index.texcoord_index * 2 + 1]);
      }

      const Vertex vertex(position, color, normal, BARY_CENTER[i % 3], texcoord, 0.0f);

      indices->push_back((uint32_t)vertices->size());
      vertices->push_back(vertex);
    }
  }
#endif

  LOG_INFO("Num of vertices : " + std::to_string(vertices->size()));
  LOG_INFO("Num of triangles: " + std::to_string(vertices->size() / 3));
}

void ObjectLoader::readMshFile(const std::string &filePath,
                               VertexArray_t vertices,
                               IndexArray_t indices,
                               const float offsetX,
                               const float offsetY,
                               const float offsetZ) {
  std::ifstream ifstream = std::ifstream(filePath, std::ios::in);

  if (ifstream) {
    std::string buffer;

    // =========================================================================================
    // Read elements
    // =========================================================================================
    // Read num elements
    std::getline(ifstream, buffer);
    const uint32_t nElements = std::stoul(buffer);

    // Read elements
    vec_pt<uint32_t> elements = std::make_shared<std::vector<uint32_t>>();
    int nNodesPerElement = -1;
    std::vector<std::vector<uint32_t>> triangleIDs;

    for (uint32_t iElement = 0U; iElement < nElements; ++iElement) {
      std::getline(ifstream, buffer);
      std::vector<std::string> tokens = StringUtil::splitText(buffer, ' ');

      if (iElement == 0U) {
        nNodesPerElement = tokens.size();

        const auto iter = MSH_TRIANGLE_IDS.find(nNodesPerElement);
        if (iter != MSH_TRIANGLE_IDS.end()) {
          // Found
          triangleIDs = iter->second;
        } else {
          // Not found
          LOG_ERROR("Unsupported msh primitive type !");
          return;
        }

        elements->resize(nNodesPerElement * nElements);
      }

      // Calc index offset
      const int offset = nNodesPerElement * iElement;

      for (uint32_t iVertex = 0U; iVertex < nNodesPerElement; ++iVertex) {
        (*elements)[offset + iVertex] = std::stoul(tokens[iVertex]);
      }
    }
    LOG_INFO("Reading elements done.");

    // =========================================================================================
    // Read vertex coords
    // =========================================================================================
    // Read num vertices
    std::getline(ifstream, buffer);
    const uint32_t nVertices = std::stoul(buffer);

    // Read vertices
    vec_pt<float> vertexCoords = std::make_shared<std::vector<float>>();
    vertexCoords->resize(3U * nVertices);

    for (uint32_t iVertex = 0U; iVertex < nVertices; ++iVertex) {
      std::getline(ifstream, buffer);
      std::vector<std::string> tokens = StringUtil::splitText(buffer, ' ');

      const uint32_t offset = 3U * iVertex;

      (*vertexCoords)[offset + 0U] = std::stof(tokens[0]);
      (*vertexCoords)[offset + 1U] = std::stof(tokens[1]);
      (*vertexCoords)[offset + 2U] = std::stof(tokens[2]);
    }
    LOG_INFO("Reading vertex coords done.");

    // =========================================================================================
    // Triangulate
    // =========================================================================================
    vec_pt<uint32_t> triangles = std::make_shared<std::vector<uint32_t>>();
    const uint32_t nTrianglesPerElement = static_cast<uint32_t>(triangleIDs.size());
    triangles->resize(3U * nTrianglesPerElement * nElements);

    for (uint32_t iElement = 0U; iElement < nElements; ++iElement) {
      const uint32_t offsetElem = 3U * nTrianglesPerElement * iElement;
      const uint32_t offset = nNodesPerElement * iElement;

      for (uint32_t iTriangle = 0U; iTriangle < nTrianglesPerElement; ++iTriangle) {
        const uint32_t offsetTriangle = offsetElem + 3U * iTriangle;

        (*triangles)[offsetTriangle + 0U] = (*elements)[offset + triangleIDs[iTriangle][0]];
        (*triangles)[offsetTriangle + 1U] = (*elements)[offset + triangleIDs[iTriangle][1]];
        (*triangles)[offsetTriangle + 2U] = (*elements)[offset + triangleIDs[iTriangle][2]];
      }
    }

    const uint32_t nTriangles = static_cast<uint32_t>(triangles->size()) / 3U;
    LOG_INFO("nTriangles: " + std::to_string(nTriangles));
    LOG_INFO("Triangulation done.");

    // =========================================================================================
    // Extract surface
    // =========================================================================================
    const vec_pt<uint32_t> &surfaceTriangles = Geometry::extractSurfaceTriangle(300, triangles, vertexCoords);

    const uint32_t nSurfaceTriangles = static_cast<uint32_t>(surfaceTriangles->size()) / 3U;
    LOG_INFO("nSurfaceTriangles: " + std::to_string(nSurfaceTriangles));
    LOG_INFO("Surface extraction done.");

    // =========================================================================================
    // Calc face normals
    // =========================================================================================
    vecf_pt faceNormal = std::make_shared<std::vector<float>>();
    faceNormal->resize(3U * nSurfaceTriangles);

    for (uint32_t iSurfaceTriangle = 0U; iSurfaceTriangle < nSurfaceTriangles; ++iSurfaceTriangle) {
      const uint32_t offset = 3U * iSurfaceTriangle;

      const uint32_t nodeIdOffset0 = 3U * (*surfaceTriangles)[offset + 0U];
      const uint32_t nodeIdOffset1 = 3U * (*surfaceTriangles)[offset + 1U];
      const uint32_t nodeIdOffset2 = 3U * (*surfaceTriangles)[offset + 2U];

      const float relVec0X = (*vertexCoords)[nodeIdOffset1 + 0U] - (*vertexCoords)[nodeIdOffset0 + 0U];
      const float relVec0Y = (*vertexCoords)[nodeIdOffset1 + 1U] - (*vertexCoords)[nodeIdOffset0 + 1U];
      const float relVec0Z = (*vertexCoords)[nodeIdOffset1 + 2U] - (*vertexCoords)[nodeIdOffset0 + 2U];

      const float relVec1X = (*vertexCoords)[nodeIdOffset2 + 0U] - (*vertexCoords)[nodeIdOffset0 + 0U];
      const float relVec1Y = (*vertexCoords)[nodeIdOffset2 + 1U] - (*vertexCoords)[nodeIdOffset0 + 1U];
      const float relVec1Z = (*vertexCoords)[nodeIdOffset2 + 2U] - (*vertexCoords)[nodeIdOffset0 + 2U];

      // Calc outer product
      // 0:   x y z x
      // 1:   x y z x
      float normalX, normalY, normalZ;
      math::outerProduct(relVec0X, relVec0Y, relVec0Z,
                         relVec1X, relVec1Y, relVec1Z,
                         normalX, normalY, normalZ);

      // Normalize
      math::normalize(normalX, normalY, normalZ);

      // Set
      (*faceNormal)[offset + 0U] = normalX;
      (*faceNormal)[offset + 1U] = normalY;
      (*faceNormal)[offset + 2U] = normalZ;
    }

    // =========================================================================================
    // Convert data to program compat format
    // =========================================================================================
    vertices->resize(nSurfaceTriangles * 3U);
    indices->resize(nSurfaceTriangles * 3U);

    for (uint32_t iTriangle = 0U; iTriangle < nSurfaceTriangles; ++iTriangle) {
      const uint32_t offset = 3U * iTriangle;

      const uint32_t vertexIdOffset0 = 3U * (*surfaceTriangles)[offset + 0U];
      const uint32_t vertexIdOffset1 = 3U * (*surfaceTriangles)[offset + 1U];
      const uint32_t vertexIdOffset2 = 3U * (*surfaceTriangles)[offset + 2U];

      const glm::vec3 vertexCoord0 = glm::vec3((*vertexCoords)[vertexIdOffset0 + 0U],
                                               (*vertexCoords)[vertexIdOffset0 + 1U],
                                               (*vertexCoords)[vertexIdOffset0 + 2U]);
      const glm::vec3 vertexCoord1 = glm::vec3((*vertexCoords)[vertexIdOffset1 + 0U],
                                               (*vertexCoords)[vertexIdOffset1 + 1U],
                                               (*vertexCoords)[vertexIdOffset1 + 2U]);
      const glm::vec3 vertexCoord2 = glm::vec3((*vertexCoords)[vertexIdOffset2 + 0U],
                                               (*vertexCoords)[vertexIdOffset2 + 1U],
                                               (*vertexCoords)[vertexIdOffset2 + 2U]);

      const glm::vec3 vertexNormal0 = glm::vec3((*faceNormal)[offset + 0U],
                                                (*faceNormal)[offset + 1U],
                                                (*faceNormal)[offset + 2U]);
      const glm::vec3 vertexNormal1 = glm::vec3((*faceNormal)[offset + 0U],
                                                (*faceNormal)[offset + 1U],
                                                (*faceNormal)[offset + 2U]);
      const glm::vec3 vertexNormal2 = glm::vec3((*faceNormal)[offset + 0U],
                                                (*faceNormal)[offset + 1U],
                                                (*faceNormal)[offset + 2U]);

      const Vertex vertex0(vertexCoord0,
                           glm::vec3(1.0f),
                           vertexNormal0,
                           BARY_CENTER[0],
                           glm::vec2(0.0f),
                           0.0f);
      const Vertex vertex1(vertexCoord1,
                           glm::vec3(1.0f),
                           vertexNormal1,
                           BARY_CENTER[1],
                           glm::vec2(0.0f),
                           0.0f);
      const Vertex vertex2(vertexCoord2,
                           glm::vec3(1.0f),
                           vertexNormal2,
                           BARY_CENTER[2],
                           glm::vec2(0.0f),
                           0.0f);

      const uint32_t index0 = offset + 0U;
      const uint32_t index1 = offset + 1U;
      const uint32_t index2 = offset + 2U;

      (*vertices)[index0] = vertex0;
      (*vertices)[index1] = vertex1;
      (*vertices)[index2] = vertex2;

      (*indices)[index0] = index0;
      (*indices)[index1] = index1;
      (*indices)[index2] = index2;
    }

    ifstream.close();
  }

  LOG_INFO("Num of vertices : " + std::to_string(vertices->size()));
  LOG_INFO("Num of triangles: " + std::to_string(vertices->size() / 3));
}

void ObjectLoader::readPchFile(const std::string &filePath,
                               VertexArray_t vertices,
                               IndexArray_t indices,
                               const float offsetX,
                               const float offsetY,
                               const float offsetZ) {
  std::ifstream ifstream = std::ifstream(filePath, std::ios::in);

  if (ifstream) {
    std::string buffer;

    // =========================================================================================
    // Read vertex coords
    // =========================================================================================
    // Read num vertices
    std::getline(ifstream, buffer);
    const int nVertices = std::stoi(buffer);

    // Read vertices
    vecf_pt vertexCoords = std::make_shared<std::vector<float>>();
    vertexCoords->resize(3 * nVertices);

    for (int iVertex = 0; iVertex < nVertices; ++iVertex) {
      std::getline(ifstream, buffer);
      std::vector<std::string> tokens = StringUtil::splitText(buffer, ' ');

      const int offset = 3 * iVertex;

      (*vertexCoords)[offset + 0] = std::stof(tokens[0]);
      (*vertexCoords)[offset + 1] = std::stof(tokens[1]);
      (*vertexCoords)[offset + 2] = std::stof(tokens[2]);
    }
    LOG_INFO("Reading vertex coords done.");

    // =========================================================================================
    // Read elements
    // =========================================================================================
    // Read num elements
    std::getline(ifstream, buffer);
    const int nElements = std::stoi(buffer);

    // Read elements
    veci_pt elements = std::make_shared<std::vector<int>>();
    elements->resize(3 * nElements);

    for (int iElement = 0; iElement < nElements; ++iElement) {
      std::getline(ifstream, buffer);
      std::vector<std::string> tokens = StringUtil::splitText(buffer, ' ');

      const int offset = 3 * iElement;

      (*elements)[offset + 0] = std::stoi(tokens[0]);
      (*elements)[offset + 1] = std::stoi(tokens[1]);
      (*elements)[offset + 2] = std::stoi(tokens[2]);
    }
    LOG_INFO("Reading elements done.");

    // =========================================================================================
    // Calc face normals
    // =========================================================================================
    vecf_pt faceNormal = std::make_shared<std::vector<float>>();
    faceNormal->resize(3 * nElements);

    for (int iElement = 0; iElement < nElements; ++iElement) {
      const int offset = 3 * iElement;

      const int nodeId0 = (*elements)[offset + 0];
      const int nodeId1 = (*elements)[offset + 1];
      const int nodeId2 = (*elements)[offset + 2];

      const int nodeIdOffset0 = 3 * nodeId0;
      const int nodeIdOffset1 = 3 * nodeId1;
      const int nodeIdOffset2 = 3 * nodeId2;

      const float relVec0X = (*vertexCoords)[nodeIdOffset1 + 0] - (*vertexCoords)[nodeIdOffset0 + 0];
      const float relVec0Y = (*vertexCoords)[nodeIdOffset1 + 1] - (*vertexCoords)[nodeIdOffset0 + 1];
      const float relVec0Z = (*vertexCoords)[nodeIdOffset1 + 2] - (*vertexCoords)[nodeIdOffset0 + 2];

      const float relVec1X = (*vertexCoords)[nodeIdOffset2 + 0] - (*vertexCoords)[nodeIdOffset0 + 0];
      const float relVec1Y = (*vertexCoords)[nodeIdOffset2 + 1] - (*vertexCoords)[nodeIdOffset0 + 1];
      const float relVec1Z = (*vertexCoords)[nodeIdOffset2 + 2] - (*vertexCoords)[nodeIdOffset0 + 2];

      // Calc outer product
      // 0:   x y z x
      // 1:   x y z x
      float normalX, normalY, normalZ;
      math::outerProduct(relVec0X, relVec0Y, relVec0Z,
                         relVec1X, relVec1Y, relVec1Z,
                         normalX, normalY, normalZ);

      // Normalize
      math::normalize(normalX, normalY, normalZ);

      // Set
      (*faceNormal)[offset + 0] = normalX;
      (*faceNormal)[offset + 1] = normalY;
      (*faceNormal)[offset + 2] = normalZ;
    }

    // =========================================================================================
    // Calc vertex normal
    // =========================================================================================
#if defined(CALC_VERTEX_NORMAL_FROM_NEIGHBOR_FACES)
    veci_pt countNodes = std::make_shared<std::vector<int>>();
    countNodes->resize(nVertices);
    for (int iNode = 0; iNode < nVertices; ++iNode) {
      (*countNodes)[iNode] = 0;
    }
    for (int iElement = 0; iElement < nElements; ++iElement) {
      const int offset = 3 * iElement;
      (*countNodes)[(*elements)[offset + 0]] = (*countNodes)[(*elements)[offset + 0]] + 1;
      (*countNodes)[(*elements)[offset + 1]] = (*countNodes)[(*elements)[offset + 1]] + 1;
      (*countNodes)[(*elements)[offset + 2]] = (*countNodes)[(*elements)[offset + 2]] + 1;
    }

    veci_pt cumsumCountNodes = std::make_shared<std::vector<int>>();
    cumsumCountNodes->resize(nVertices);

    int cumsum = 0;
    for (int iNode = 0; iNode < nVertices; ++iNode) {
      (*cumsumCountNodes)[iNode] = cumsum;
      cumsum += (*countNodes)[iNode];
    }

    // Calc belong to array
    veci_pt belongTo = std::make_shared<std::vector<int>>();
    belongTo->resize(cumsum);

    veci_pt belongToCounter = std::make_shared<std::vector<int>>();
    belongToCounter->resize(nVertices);

    for (int iNode = 0; iNode < nVertices; ++iNode) {
      (*belongToCounter)[iNode] = 0;
    }

    for (int iElement = 0; iElement < nElements; ++iElement) {
      const int offset = 3 * iElement;

      const int nodeId0 = (*elements)[offset + 0];
      const int nodeId1 = (*elements)[offset + 1];
      const int nodeId2 = (*elements)[offset + 2];

      const int offset0 = (*cumsumCountNodes)[nodeId0] + (*belongToCounter)[nodeId0];
      (*belongTo)[offset0] = iElement;
      (*belongToCounter)[nodeId0] = (*belongToCounter)[nodeId0] + 1;

      const int offset1 = (*cumsumCountNodes)[nodeId1] + (*belongToCounter)[nodeId1];
      (*belongTo)[offset1] = iElement;
      (*belongToCounter)[nodeId1] = (*belongToCounter)[nodeId1] + 1;

      const int offset2 = (*cumsumCountNodes)[nodeId2] + (*belongToCounter)[nodeId2];
      (*belongTo)[offset2] = iElement;
      (*belongToCounter)[nodeId2] = (*belongToCounter)[nodeId2] + 1;
    }

    // Calc vertex normals
    vecf_pt vertexNormal = std::make_shared<std::vector<float>>();
    vertexNormal->resize(3 * nVertices);

    for (int iVertex = 0; iVertex < nVertices; ++iVertex) {
      const int offset = 3 * iVertex;

      const int belongToOffset = (*cumsumCountNodes)[iVertex];
      const int nBelongTo = (*belongToCounter)[iVertex];

      float vertexNormalX = 0.0f;
      float vertexNormalY = 0.0f;
      float vertexNormalZ = 0.0f;

      for (int iBelongTo = 0; iBelongTo < nBelongTo; ++iBelongTo) {
        const int elementId = (*belongTo)[belongToOffset + iBelongTo];
        const int elementOffset = 3 * elementId;

        vertexNormalX += (*faceNormal)[elementOffset + 0];
        vertexNormalY += (*faceNormal)[elementOffset + 1];
        vertexNormalZ += (*faceNormal)[elementOffset + 2];
      }

      vertexNormalX /= (float)nBelongTo;
      vertexNormalY /= (float)nBelongTo;
      vertexNormalZ /= (float)nBelongTo;

      // Normalize
      const float length = std::sqrt(vertexNormalX * vertexNormalX + vertexNormalY * vertexNormalY + vertexNormalZ * vertexNormalZ);
      vertexNormalX /= length;
      vertexNormalY /= length;
      vertexNormalZ /= length;

      (*vertexNormal)[offset + 0] = vertexNormalX;
      (*vertexNormal)[offset + 1] = vertexNormalY;
      (*vertexNormal)[offset + 2] = vertexNormalZ;
    }
#endif

    // =========================================================================================
    // Convert data to program compat format
    // =========================================================================================
    vertices->resize(nElements * 3);
    indices->resize(nElements * 3);

    for (int iElement = 0; iElement < nElements; ++iElement) {
      const int offset = 3 * iElement;
      const int offset0 = offset + 0;
      const int offset1 = offset + 1;
      const int offset2 = offset + 2;

      const int vertexIdOffset0 = 3 * (*elements)[offset0];
      const int vertexIdOffset1 = 3 * (*elements)[offset1];
      const int vertexIdOffset2 = 3 * (*elements)[offset2];

      const glm::vec3 vertexCoord0 = glm::vec3((*vertexCoords)[vertexIdOffset0 + 0],
                                               (*vertexCoords)[vertexIdOffset0 + 1],
                                               (*vertexCoords)[vertexIdOffset0 + 2]);
      const glm::vec3 vertexCoord1 = glm::vec3((*vertexCoords)[vertexIdOffset1 + 0],
                                               (*vertexCoords)[vertexIdOffset1 + 1],
                                               (*vertexCoords)[vertexIdOffset1 + 2]);
      const glm::vec3 vertexCoord2 = glm::vec3((*vertexCoords)[vertexIdOffset2 + 0],
                                               (*vertexCoords)[vertexIdOffset2 + 1],
                                               (*vertexCoords)[vertexIdOffset2 + 2]);

#if defined(CALC_VERTEX_NORMAL_FROM_NEIGHBOR_FACES)
      const glm::vec3 vertexNormal0 = glm::vec3((*vertexNormal)[vertexIdOffset0 + 0],
                                                (*vertexNormal)[vertexIdOffset0 + 1],
                                                (*vertexNormal)[vertexIdOffset0 + 2]);
      const glm::vec3 vertexNormal1 = glm::vec3((*vertexNormal)[vertexIdOffset1 + 0],
                                                (*vertexNormal)[vertexIdOffset1 + 1],
                                                (*vertexNormal)[vertexIdOffset1 + 2]);
      const glm::vec3 vertexNormal2 = glm::vec3((*vertexNormal)[vertexIdOffset2 + 0],
                                                (*vertexNormal)[vertexIdOffset2 + 1],
                                                (*vertexNormal)[vertexIdOffset2 + 2]);
#else
      const glm::vec3 vertexNormal0 = glm::vec3((*faceNormal)[offset + 0],
                                                (*faceNormal)[offset + 1],
                                                (*faceNormal)[offset + 2]);
      const glm::vec3 vertexNormal1 = glm::vec3((*faceNormal)[offset + 0],
                                                (*faceNormal)[offset + 1],
                                                (*faceNormal)[offset + 2]);
      const glm::vec3 vertexNormal2 = glm::vec3((*faceNormal)[offset + 0],
                                                (*faceNormal)[offset + 1],
                                                (*faceNormal)[offset + 2]);
#endif

      const Vertex vertex0(vertexCoord0,
                           glm::vec3(1.0f),
                           vertexNormal0,
                           BARY_CENTER[0],
                           glm::vec2(0.0f),
                           0.0f);
      const Vertex vertex1(vertexCoord1,
                           glm::vec3(1.0f),
                           vertexNormal1,
                           BARY_CENTER[1],
                           glm::vec2(0.0f),
                           0.0f);
      const Vertex vertex2(vertexCoord2,
                           glm::vec3(1.0f),
                           vertexNormal2,
                           BARY_CENTER[2],
                           glm::vec2(0.0f),
                           0.0f);

      (*vertices)[offset0] = vertex0;
      (*vertices)[offset1] = vertex1;
      (*vertices)[offset2] = vertex2;

      (*indices)[offset0] = offset0;
      (*indices)[offset1] = offset1;
      (*indices)[offset2] = offset2;
    }
  }

  LOG_INFO("Num of vertices : " + std::to_string(vertices->size()));
  LOG_INFO("Num of triangles: " + std::to_string(vertices->size() / 3));
}

void ObjectLoader::readLasFile(const std::string &filePath,
                               VertexArray_t vertices,
                               IndexArray_t indices,
                               const float offsetX,
                               const float offsetY,
                               const float offsetZ) {
  const auto pointData = llas::read(filePath, true);

  if (pointData != nullptr) {
    const size_t nPoints = pointData->getNumPoints();
    const auto &coords = pointData->getPointCoords();
    const auto &colors = pointData->getPointColors();

    vertices->resize(nPoints);
    indices->resize(nPoints);

    for (size_t iPoint = 0; iPoint < nPoints; ++iPoint) {
      const size_t offset = 3 * iPoint;
      const Vertex vertex(
          glm::vec3((float)coords[offset + 0],
                    (float)coords[offset + 1],
                    (float)coords[offset + 2]),
          glm::vec3((float)colors[offset + 0] / 255.0f,
                    (float)colors[offset + 1] / 255.0f,
                    (float)colors[offset + 2] / 255.0f),
          glm::vec3(0.0f),
          glm::vec3(0.0f),
          glm::vec2(0.0f),
          0.0f);

      (*vertices)[iPoint] = vertex;
      (*indices)[iPoint] = (uint32_t)iPoint;
    }
  }

  LOG_INFO("Num of points : " + std::to_string(vertices->size()));
}

void ObjectLoader::readVtkFile(const std::string &filePath,
                               VertexArray_t vertices,
                               IndexArray_t indices,
                               const float offsetX,
                               const float offsetY,
                               const float offsetZ) {
  if (!FileUtil::exists(filePath)) {
    LOG_ERROR("File not found: " + filePath);
    return;
  }

#if defined(SIMVIEW_WITH_VTK)
  vtkUnstructuredGrid *unstructuredGrid = nullptr;
  const std::string extension = FileUtil::extension(filePath);

  if (extension == ".vtk") {
    vtkSmartPointer<vtkUnstructuredGridReader> reader = vtkSmartPointer<vtkUnstructuredGridReader>::New();
    reader->SetFileName(filePath.c_str());
    reader->Update();

    const auto content = reader->GetOutput();
    if (content != nullptr) {
      unstructuredGrid = vtkUnstructuredGrid::New();
      unstructuredGrid->ShallowCopy(content);
    }
  } else if (extension == ".vtu") {
    vtkSmartPointer<vtkXMLUnstructuredGridReader> reader = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
    reader->SetFileName(filePath.c_str());
    reader->Update();

    const auto content = reader->GetOutput();
    if (content != nullptr) {
      unstructuredGrid = vtkUnstructuredGrid::New();
      unstructuredGrid->ShallowCopy(content);
    }
  } else {
    LOG_ERROR("Unsupported VTK file extension: " + extension);
    return;
  }

  if (unstructuredGrid != nullptr) {
    // Triangrated Faces
    {
      vtkPoints *points = unstructuredGrid->GetPoints();
      vtkCellArray *cells = unstructuredGrid->GetCells();

      if (points != nullptr && cells != nullptr) {
        const vtkIdType nCells = cells->GetNumberOfCells();
        size_t vertexId = 0;

        for (vtkIdType cellId = 0; cellId < nCells; ++cellId) {
          vtkSmartPointer<vtkIdList> ptIds = vtkSmartPointer<vtkIdList>::New();
          vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();

          vtkCell *cell = unstructuredGrid->GetCell(cellId);
          const int cellType = cell->GetCellType();
          vtkIdList *idList = cell->GetPointIds();
          const int nVertices = idList->GetNumberOfIds();
          double coordsBuffer[3] = {0.0, 0.0, 0.0};

          if (cellType == VTK_TRIANGLE) {
            for (size_t iTriangle = 0; iTriangle < VTK_TRIANGLE_IDS_TRIANGLE.size(); ++iTriangle) {
              for (size_t iVertex = 0; iVertex < 3; ++iVertex) {
                const vtkIdType pointId = (uint32_t)idList->GetId(VTK_TRIANGLE_IDS_TRIANGLE[iTriangle][iVertex]);

                // Get vertex coords
                points->GetPoint(pointId, coordsBuffer);

                Vertex vertex(glm::vec3(coordsBuffer[0], coordsBuffer[1], coordsBuffer[2]),
                              glm::vec3(0.0f),
                              glm::vec3(0.0f),
                              BARY_CENTER[iVertex],
                              glm::vec2(0.0f),
                              0.0f);

                vertices->push_back(vertex);
                indices->push_back(vertexId++);
              }
            }
          } else if (cellType == VTK_QUAD) {
            for (size_t iTriangle = 0; iTriangle < VTK_TRIANGLE_IDS_QUAD.size(); ++iTriangle) {
              for (size_t iVertex = 0; iVertex < 3; ++iVertex) {
                const vtkIdType pointId = (uint32_t)idList->GetId(VTK_TRIANGLE_IDS_QUAD[iTriangle][iVertex]);

                // Get vertex coords
                points->GetPoint(pointId, coordsBuffer);

                Vertex vertex(glm::vec3(coordsBuffer[0], coordsBuffer[1], coordsBuffer[2]),
                              glm::vec3(0.0f),
                              glm::vec3(0.0f),
                              BARY_CENTER[iVertex],
                              glm::vec2(0.0f),
                              0.0f);

                vertices->push_back(vertex);
                indices->push_back(vertexId++);
              }
            }
          } else if (cellType == VTK_TETRA) {
            for (size_t iTriangle = 0; iTriangle < VTK_TRIANGLE_IDS_TETRA.size(); ++iTriangle) {
              for (size_t iVertex = 0; iVertex < 3; ++iVertex) {
                const vtkIdType pointId = (uint32_t)idList->GetId(VTK_TRIANGLE_IDS_TETRA[iTriangle][iVertex]);

                // Get vertex coords
                points->GetPoint(pointId, coordsBuffer);

                Vertex vertex(glm::vec3(coordsBuffer[0], coordsBuffer[1], coordsBuffer[2]),
                              glm::vec3(0.0f),
                              glm::vec3(0.0f),
                              BARY_CENTER[iVertex],
                              glm::vec2(0.0f),
                              0.0f);

                vertices->push_back(vertex);
                indices->push_back(vertexId++);
              }
            }
          } else if (cellType == VTK_HEXAHEDRON) {
            for (size_t iTriangle = 0; iTriangle < VTK_TRIANGLE_IDS_HEXAHEDRON.size(); ++iTriangle) {
              for (size_t iVertex = 0; iVertex < 3; ++iVertex) {
                const vtkIdType pointId = (uint32_t)idList->GetId(VTK_TRIANGLE_IDS_HEXAHEDRON[iTriangle][iVertex]);

                // Get vertex coords
                points->GetPoint(pointId, coordsBuffer);

                Vertex vertex(glm::vec3(coordsBuffer[0], coordsBuffer[1], coordsBuffer[2]),
                              glm::vec3(0.0f),
                              glm::vec3(0.0f),
                              BARY_CENTER[iVertex],
                              glm::vec2(0.0f),
                              0.0f);

                vertices->push_back(vertex);
                indices->push_back(vertexId++);
              }
            }
          } else if (cellType == VTK_WEDGE) {
            for (size_t iTriangle = 0; iTriangle < VTK_TRIANGLE_IDS_WEDGE.size(); ++iTriangle) {
              for (size_t iVertex = 0; iVertex < 3; ++iVertex) {
                const vtkIdType pointId = (uint32_t)idList->GetId(VTK_TRIANGLE_IDS_WEDGE[iTriangle][iVertex]);

                // Get vertex coords
                points->GetPoint(pointId, coordsBuffer);

                Vertex vertex(glm::vec3(coordsBuffer[0], coordsBuffer[1], coordsBuffer[2]),
                              glm::vec3(0.0f),
                              glm::vec3(0.0f),
                              BARY_CENTER[iVertex],
                              glm::vec2(0.0f),
                              0.0f);

                vertices->push_back(vertex);
                indices->push_back(vertexId++);
              }
            }
          } else {
            LOG_WARN("Unsupported cell type!");
            break;
          }  // end of if 'cellType'
        }    // end of for-loop 'cellid'
      }      // end of null-check 'cells'
    }
  } else {
    LOG_ERROR("Unstructured grid data is null!");
  }
#endif

  LOG_INFO("Num of vertices : " + std::to_string(vertices->size()));
  LOG_INFO("Num of triangles: " + std::to_string(vertices->size() / 3));
}

void ObjectLoader::readObjFileWithMaterialGroup(const std::string &filePath,
                                                MaterialGroups_t materialGroups,
                                                const glm::vec3 offset,
                                                const glm::vec3 scale) {
  unsigned int nMaterials = 0;
  unsigned int nFaces = 0;
  unsigned int nVertices = 0;

#if defined(SIMVIEW_WITH_ASSIMP)
  Assimp::Importer importer;
  unsigned int flag = 0;
  flag |= aiProcess_Triangulate;
  flag |= aiProcess_CalcTangentSpace;
  flag |= aiProcess_RemoveRedundantMaterials;
  flag |= aiProcess_GenNormals;

  const aiScene *scene = importer.ReadFile(filePath, flag);

  if (scene == nullptr) {
    LOG_ERROR("[ERROR] " + std::string(importer.GetErrorString()));
    return;
  }

  const std::string mtlFileDir = FileUtil::dirPath(filePath);

  for (int iMaterial = 0; iMaterial < scene->mNumMaterials; ++iMaterial) {
    nMaterials++;

    aiMaterial *material = scene->mMaterials[iMaterial];
    auto materialGroup = std::make_shared<MaterialGroup>();

    // Name
    aiString name;
    material->Get(AI_MATKEY_NAME, name);
    LOG_INFO("Material name: " + std::string(name.C_Str()));

    ai_int illuminationModel;
    material->Get(AI_MATKEY_SHADING_MODEL, illuminationModel);
    LOG_INFO("Material name: " + std::string(name.C_Str()) + ", illum: " + std::to_string(illuminationModel));

    // Colors
    aiColor3D ambientColor, diffuseColor, specularColor;
    material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
    material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);

    materialGroup->ambientColor = glm::vec3(ambientColor.r, ambientColor.g, ambientColor.b);
    materialGroup->diffuseColor = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
    materialGroup->specularColor = glm::vec3(specularColor.r, specularColor.g, specularColor.b);

    material->Get(AI_MATKEY_SHININESS, materialGroup->shininess);

    aiString ambientTexturePath;
    aiString diffuseTexturePath;
    aiString specularTexturePath;
    aiString specularReflectionTexturePath;
    aiString opacityTexturePath;
    aiString roughnessTexturePath;
    aiString bumpTexturePath;
    aiString reflectionDiffuseTexturePath;

    material->GetTexture(aiTextureType_AMBIENT, 0, &ambientTexturePath);
    material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTexturePath);
    material->GetTexture(aiTextureType_SPECULAR, 0, &specularTexturePath);
    material->GetTexture(aiTextureType_METALNESS, 0, &specularReflectionTexturePath);
    material->GetTexture(aiTextureType_OPACITY, 0, &opacityTexturePath);
    material->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &roughnessTexturePath);
    material->GetTexture(aiTextureType_NORMALS, 0, &bumpTexturePath);
    material->GetTexture(aiTextureType_REFLECTION, 0, &reflectionDiffuseTexturePath);

    materialGroup->ambientTexturePath = FileUtil::join(mtlFileDir, std::string(ambientTexturePath.C_Str()));
    materialGroup->diffuseTexturePath = FileUtil::join(mtlFileDir, std::string(diffuseTexturePath.C_Str()));
    materialGroup->specularTexturePath = FileUtil::join(mtlFileDir, std::string(specularTexturePath.C_Str()));
    materialGroup->specularReflectionTexturePath = FileUtil::join(mtlFileDir, std::string(specularReflectionTexturePath.C_Str()));
    materialGroup->opacityTexturePath = FileUtil::join(mtlFileDir, std::string(opacityTexturePath.C_Str()));
    materialGroup->roughnessTexturePath = FileUtil::join(mtlFileDir, std::string(roughnessTexturePath.C_Str()));
    materialGroup->bumpTexturePath = FileUtil::join(mtlFileDir, std::string(bumpTexturePath.C_Str()));
    materialGroup->reflectionDiffuseTexturePath = FileUtil::join(mtlFileDir, std::string(reflectionDiffuseTexturePath.C_Str()));

    materialGroups->push_back(materialGroup);
  }

  bool isFoundMaterials = (int)materialGroups->size() != 0;
  if (!isFoundMaterials) {
    materialGroups->push_back(std::make_shared<MaterialGroup>());
  }

  for (int iMesh = 0; iMesh < scene->mNumMeshes; ++iMesh) {
    aiMesh *mesh = scene->mMeshes[iMesh];

    for (int iFace = 0; iFace < mesh->mNumFaces; ++iFace) {
      nFaces++;

      const aiFace &face = mesh->mFaces[iFace];

      for (int iVertex = 0; iVertex < face.mNumIndices; ++iVertex) {
        nVertices++;

        const unsigned int index = face.mIndices[iVertex];

        glm::vec3 position = glm::vec3(mesh->mVertices[index].x, mesh->mVertices[index].y, mesh->mVertices[index].z);
        glm::vec3 normal(0.0f), color(1.0f);
        glm::vec2 texcoord(0.0f);

        if (mesh->HasNormals()) {
          normal = glm::vec3(mesh->mNormals[index].x, mesh->mNormals[index].y, mesh->mNormals[index].z);
        }

        if (mesh->HasTextureCoords(0)) {
          texcoord = glm::vec2(mesh->mTextureCoords[0][index].x, mesh->mTextureCoords[0][index].y);
        }

        const unsigned int materialID = mesh->mMaterialIndex;

        const Vertex vertex(position,
                            color,
                            normal,
                            BARY_CENTER[iVertex % 3],
                            texcoord,
                            0.0f);

        (*materialGroups)[materialID]->indices->push_back(uint32_t((*materialGroups)[materialID]->vertices->size()));
        (*materialGroups)[materialID]->vertices->push_back(vertex);
      }
    }
  }
#else
  const std::string mtlFileDir = FileUtil::dirPath(filePath);
  LOG_INFO("### Mtl file dir : " + mtlFileDir);

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;

  bool success = tinyobj::LoadObj(
      &attrib,
      &shapes,
      &materials,
      &warn,
      &err,
      filePath.c_str(),
      mtlFileDir.c_str());

  if (!warn.empty()) {
    LOG_WARN("[WARNING] " + warn);
  }

  if (!err.empty()) {
    LOG_ERROR("[Error] " + err);
  }

  if (!success) {
    throw std::runtime_error("Failed to load OBJ file: " + filePath);
  }

  // Load Material
  for (const auto &material : materials) {
    nMaterials++;

    auto materialGroup = std::make_shared<MaterialGroup>();

    materialGroup->ambientColor = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]);
    materialGroup->diffuseColor = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
    materialGroup->specularColor = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
    materialGroup->shininess = material.shininess;

    std::string diffuseTexturePath = material.diffuse_texname;
    if (!diffuseTexturePath.empty() && !FileUtil::isAbsolute(diffuseTexturePath)) {
      diffuseTexturePath = FileUtil::join(FileUtil::dirPath(filePath), diffuseTexturePath);
    }
    materialGroup->diffuseTexturePath = diffuseTexturePath;

    materialGroups->push_back(materialGroup);
  }

  LOG_INFO("Loaded " + std::to_string(materialGroups->size()) + " materials.");

  bool isFoundMaterials = (int)materialGroups->size() != 0;
  if (!isFoundMaterials) {
    materialGroups->push_back(std::make_shared<MaterialGroup>());
  }

  // Load Object
  for (const auto &shape : shapes) {
    size_t indexOffset = 0;

    for (size_t iFace = 0; iFace < shape.mesh.num_face_vertices.size(); ++iFace) {
      nFaces++;

      const int nVertices = shape.mesh.num_face_vertices[iFace];
      const int materialID = isFoundMaterials ? shape.mesh.material_ids[iFace] : 0;

      for (size_t iVertex = 0; iVertex < nVertices; ++iVertex) {
        // access to vertex
        const tinyobj::index_t index = shape.mesh.indices[indexOffset + iVertex];

        glm::vec3 position(0.0f), normal(0.0f), color(1.0f);
        glm::vec2 texcoord(0.0f);

        if (index.vertex_index >= 0) {
          position = glm::vec3(
              attrib.vertices[3 * index.vertex_index + 0],
              attrib.vertices[3 * index.vertex_index + 1],
              attrib.vertices[3 * index.vertex_index + 2]);
          color = glm::vec3(
              attrib.colors[3 * index.vertex_index + 0],
              attrib.colors[3 * index.vertex_index + 1],
              attrib.colors[3 * index.vertex_index + 2]);
        }

        if (index.normal_index >= 0) {
          normal = glm::vec3(
              attrib.normals[3 * index.normal_index + 0],
              attrib.normals[3 * index.normal_index + 1],
              attrib.normals[3 * index.normal_index + 2]);
        }

        if (index.texcoord_index >= 0) {
          texcoord = glm::vec2(
              attrib.texcoords[2 * index.texcoord_index + 0],
              attrib.texcoords[2 * index.texcoord_index + 1]);
        }

        const Vertex vertex(position,
                            color,
                            normal,
                            BARY_CENTER[iVertex % 3],
                            texcoord,
                            0.0f);

        (*materialGroups)[materialID]->indices->push_back(uint32_t((*materialGroups)[materialID]->vertices->size()));
        (*materialGroups)[materialID]->vertices->push_back(vertex);
      }

      indexOffset += nVertices;
    }
  }
#endif

  {
    //// Scale and Translate
    // VertexArray_t allVertices = std::make_shared<std::vector<Vertex>>();
    // IndexArray_t allIndices = std::make_shared<std::vector<uint32_t>>();
    // for (const auto &materialGroup : *materialGroups) {
    //   mergeVertices(materialGroup->vertices, materialGroup->indices, allVertices, allIndices);
    // }

    // glm::vec3 minCoord, maxCoord;
    // std::tie(minCoord, maxCoord) = getCorners(allVertices);
    // glm::vec3 centerCoord = (minCoord + maxCoord) / 2.0f;

    //// To origin
    // for (auto &materialGroup : *materialGroups) {
    //   ObjectLoader::translateObject(materialGroup->vertices, -centerCoord);
    // }

    //// Scale
    // for (auto &materialGroup : *materialGroups) {
    //   ObjectLoader::scaleObject(materialGroup->vertices, scale);
    // }

    //// Translate
    // for (auto &materialGroup : *materialGroups) {
    //   ObjectLoader::translateObject(materialGroup->vertices, offset);
    // }
  }

  LOG_INFO("Num of vertices : " + std::to_string(nVertices));
  LOG_INFO("Num of triangles: " + std::to_string(nFaces));
  LOG_INFO("Num of materials: " + std::to_string(nMaterials));
}

void ObjectLoader::scaleObject(VertexArray_t vertices,
                               const float scale) {
  ObjectLoader::scaleObject(vertices, scale, scale, scale);
}

void ObjectLoader::scaleObject(VertexArray_t vertices,
                               const float scaleX,
                               const float scaleY,
                               const float scaleZ) {
  glm::vec3 minCoords, maxCoords;
  std::tie(minCoords, maxCoords) = getCorners(vertices);

  const glm::vec3 center = (maxCoords + minCoords) / 2.0f;
  const glm::vec3 range = maxCoords - minCoords;
  const glm::vec3 scale(scaleX, scaleY, scaleZ);

  for (int i = 0; i < vertices->size(); ++i) {
    (*vertices)[i].position = ((*vertices)[i].position - center) * scale + center;
  }
}

void ObjectLoader::scaleObject(VertexArray_t vertices,
                               const glm::vec3 scale) {
  ObjectLoader::scaleObject(vertices, scale.x, scale.y, scale.z);
}

void ObjectLoader::moveToOrigin(VertexArray_t vertices) {
  glm::vec3 minCoords, maxCoords;
  std::tie(minCoords, maxCoords) = getCorners(vertices);

  glm::vec3 center = (maxCoords + minCoords) / 2.0f;

  for (int i = 0; i < vertices->size(); ++i) {
    (*vertices)[i].position = (*vertices)[i].position - center;
  }
}

void ObjectLoader::translateObject(VertexArray_t vertices,
                                   const float offsetX,
                                   const float offsetY,
                                   const float offsetZ) {
  glm::vec3 offset(offsetX, offsetY, offsetZ);

  for (int i = 0; i < vertices->size(); ++i) {
    (*vertices)[i].position = (*vertices)[i].position + offset;
  }
}

void ObjectLoader::translateObject(VertexArray_t vertices,
                                   const glm::vec3 offset) {
  ObjectLoader::translateObject(vertices, offset.x, offset.y, offset.z);
}

void ObjectLoader::rotateObject(VertexArray_t vertices, const float angle, const glm::vec3 axis) {
  const int nVertices = vertices->size();
  const glm::mat4 rotMat = glm::rotate(angle, axis);

  for (int i_vertex = 0; i_vertex < nVertices; ++i_vertex) {
    const glm::vec4 &position = rotMat * glm::vec4((*vertices)[i_vertex].position, 1.0f);
    const glm::vec4 &normal = rotMat * glm::vec4((*vertices)[i_vertex].normal, 1.0f);
    (*vertices)[i_vertex].position = glm::vec3(position.x, position.y, position.z);
    (*vertices)[i_vertex].normal = glm::vec3(normal.x, normal.y, normal.z);
  }
};

void ObjectLoader::mergeVertices(
    const VertexArray_t &sourceVertices,
    const IndexArray_t &sourceIndices,
    VertexArray_t &distVertices,
    IndexArray_t &distIndices) {
  const int offsetIndex = distIndices->size();
  const int nSourceVertices = sourceVertices->size();
  const int distTotalSize = offsetIndex + nSourceVertices;

  distVertices->resize(distTotalSize);
  distIndices->resize(distTotalSize);

  for (int i_vertex = 0; i_vertex < nSourceVertices; ++i_vertex) {
    (*distVertices)[offsetIndex + i_vertex] = (*sourceVertices)[i_vertex];
    (*distIndices)[offsetIndex + i_vertex] = (*sourceIndices)[i_vertex] + offsetIndex;
  }
};

std::pair<glm::vec3, glm::vec3> ObjectLoader::getCorners(VertexArray_t vertices) {
  glm::vec3 maxCoords(0.0f);
  glm::vec3 minCoords(0.0f);

  for (int i = 0; i < vertices->size(); ++i) {
    const Vertex &vertex = (*vertices)[i];

    for (int direction = 0; direction < 3; ++direction) {
      if (i == 0) {
        minCoords[direction] = vertex.position[direction];
        maxCoords[direction] = vertex.position[direction];
        continue;
      }

      if (vertex.position[direction] > maxCoords[direction]) {
        maxCoords[direction] = vertex.position[direction];
      }

      if (vertex.position[direction] < minCoords[direction]) {
        minCoords[direction] = vertex.position[direction];
      }
    }
  }

  return {std::move(minCoords), std::move(maxCoords)};
}

}  // namespace util
}  // namespace simview