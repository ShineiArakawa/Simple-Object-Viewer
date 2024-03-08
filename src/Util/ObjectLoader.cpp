#include <Util/ObjectLoader.hpp>

#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#endif

namespace simview {
namespace util {

void ObjectLoader::readFromFile(const std::string &filePath,
                                VertexArray_t vertices,
                                IndexArray_t indices,
                                const float offsetX,
                                const float offsetY,
                                const float offsetZ) {
  const std::string extension = FileUtil::extension(filePath);

  LOG_INFO("### Start load obj file: " + filePath);

#if defined(USE_ASSIMP)
  if (extension == ".obj" || extension == ".stl") {
#else
  if (extension == ".obj") {
#endif
    readObjFile(filePath, vertices, indices, offsetX, offsetY, offsetZ);
  } else if (extension == ".msh") {
    readMshFile(filePath, vertices, indices, offsetX, offsetY, offsetZ);
  } else if (extension == ".pch") {
    readPchFile(filePath, vertices, indices, offsetX, offsetY, offsetZ);
  } else if (extension == ".las") {
    readLazFile(filePath, vertices, indices, offsetX, offsetY, offsetZ);
  } else {
    LOG_ERROR("Unsupprted file type: " + filePath);
    return;
  }

  LOG_INFO("### Done.");
}

void ObjectLoader::readObjFile(const std::string &filePath,
                               VertexArray_t vertices,
                               IndexArray_t indices,
                               const float offsetX,
                               const float offsetY,
                               const float offsetZ) {
#if defined(USE_ASSIMP)
  Assimp::Importer importer;
  unsigned int flag = 0;
  flag |= aiProcess_Triangulate;
  flag |= aiProcess_CalcTangentSpace;
  flag |= aiProcess_RemoveRedundantMaterials;

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

  // Vertex配列の作成
  // Create vertex array

  glm::vec3 maxCoords(0.0f);
  glm::vec3 minCoords(0.0f);

  for (int s = 0; s < shapes.size(); s++) {
    const tinyobj::mesh_t &mesh = shapes[s].mesh;

    int nVertices = mesh.indices.size();

    for (int i = 0; i < nVertices; i++) {
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

  ObjectLoader::moveToOrigin(vertices);
  ObjectLoader::translateObject(vertices, offsetX, offsetY, offsetZ);

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

  const int faces[16][3] = {
      {0, 6, 5},
      {6, 3, 8},
      {8, 5, 6},
      {5, 8, 2},
      {0, 5, 4},
      {5, 2, 7},
      {7, 4, 5},
      {4, 7, 1},
      {0, 4, 6},
      {4, 1, 9},
      {9, 6, 4},
      {6, 9, 3},
      {1, 7, 9},
      {7, 2, 8},
      {8, 9, 7},
      {9, 8, 3},
  };

  if (ifstream) {
    std::string buffer;
    std::vector<std::string> tokens;

    // =========================================================================================
    // Read elements
    // =========================================================================================
    // Read num elements
    std::getline(ifstream, buffer);
    const int nElements = std::stoi(buffer);

    // Read elements
    veci_pt elements = std::make_shared<std::vector<int>>();
    elements->resize(10 * nElements);

    for (int iElement = 0; iElement < nElements; ++iElement) {
      std::getline(ifstream, buffer);
      splitText(buffer, ' ', tokens);

      const int offset = 10 * iElement;

      (*elements)[offset + 0] = std::stoi(tokens[0]);
      (*elements)[offset + 1] = std::stoi(tokens[1]);
      (*elements)[offset + 2] = std::stoi(tokens[2]);
      (*elements)[offset + 3] = std::stoi(tokens[3]);
      (*elements)[offset + 4] = std::stoi(tokens[4]);
      (*elements)[offset + 5] = std::stoi(tokens[5]);
      (*elements)[offset + 6] = std::stoi(tokens[6]);
      (*elements)[offset + 7] = std::stoi(tokens[7]);
      (*elements)[offset + 8] = std::stoi(tokens[8]);
      (*elements)[offset + 9] = std::stoi(tokens[9]);
    }
    LOG_INFO("Reading elements done.");

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
      splitText(buffer, ' ', tokens);

      const int offset = 3 * iVertex;

      (*vertexCoords)[offset + 0] = std::stof(tokens[0]);
      (*vertexCoords)[offset + 1] = std::stof(tokens[1]);
      (*vertexCoords)[offset + 2] = std::stof(tokens[2]);
    }
    LOG_INFO("Reading vertex coords done.");

    // =========================================================================================
    // Triangulate
    // =========================================================================================
    veci_pt triangles = std::make_shared<std::vector<int>>();
    triangles->resize(3 * 16 * nElements);

    for (int iElement = 0; iElement < nElements; ++iElement) {
      const int offsetElem = 3 * 16 * iElement;
      const int offset = 10 * iElement;

      for (int iTriangle = 0; iTriangle < 16; ++iTriangle) {
        const int offsetTriangle = offsetElem + 3 * iTriangle;

        (*triangles)[offsetTriangle + 0] = (*elements)[offset + faces[iTriangle][0]];
        (*triangles)[offsetTriangle + 1] = (*elements)[offset + faces[iTriangle][1]];
        (*triangles)[offsetTriangle + 2] = (*elements)[offset + faces[iTriangle][2]];
      }
    }

    const int nTriangles = triangles->size() / 3;
    LOG_INFO("nTriangles: " + std::to_string(nTriangles));
    LOG_INFO("Triangulation done.");

    // =========================================================================================
    // Extract surface
    // =========================================================================================
    const veci_pt &surfaceTriangles = Geometry::extractSurfaceTriangle(250, triangles, vertexCoords);

    const int nSurfaceTriangles = surfaceTriangles->size() / 3;
    LOG_INFO("nSurfaceTriangles: " + std::to_string(nSurfaceTriangles));
    LOG_INFO("Surface extraction done.");

    // =========================================================================================
    // Convert data to program compat format
    // =========================================================================================
    vertices->resize(nSurfaceTriangles * 3);
    indices->resize(nSurfaceTriangles * 3);

    for (int iTriangle = 0; iTriangle < nSurfaceTriangles; ++iTriangle) {
      const int offset = 3 * iTriangle;

      const int vertexIdOffset0 = 3 * (*surfaceTriangles)[offset + 0];
      const int vertexIdOffset1 = 3 * (*surfaceTriangles)[offset + 1];
      const int vertexIdOffset2 = 3 * (*surfaceTriangles)[offset + 2];

      const glm::vec3 vertexCoord0 = glm::vec3((*vertexCoords)[vertexIdOffset0 + 0],
                                               (*vertexCoords)[vertexIdOffset0 + 1],
                                               (*vertexCoords)[vertexIdOffset0 + 2]);
      const glm::vec3 vertexCoord1 = glm::vec3((*vertexCoords)[vertexIdOffset1 + 0],
                                               (*vertexCoords)[vertexIdOffset1 + 1],
                                               (*vertexCoords)[vertexIdOffset1 + 2]);
      const glm::vec3 vertexCoord2 = glm::vec3((*vertexCoords)[vertexIdOffset2 + 0],
                                               (*vertexCoords)[vertexIdOffset2 + 1],
                                               (*vertexCoords)[vertexIdOffset2 + 2]);

      const Vertex vertex0(vertexCoord0,
                           glm::vec3(1.0f),
                           glm::vec3(0.0f),
                           BARY_CENTER[0],
                           glm::vec2(0.0f),
                           0.0f);
      const Vertex vertex1(vertexCoord1,
                           glm::vec3(1.0f),
                           glm::vec3(0.0f),
                           BARY_CENTER[1],
                           glm::vec2(0.0f),
                           0.0f);
      const Vertex vertex2(vertexCoord2,
                           glm::vec3(1.0f),
                           glm::vec3(0.0f),
                           BARY_CENTER[2],
                           glm::vec2(0.0f),
                           0.0f);

      const int index0 = offset + 0;
      const int index1 = offset + 1;
      const int index2 = offset + 2;

      (*vertices)[index0] = vertex0;
      (*vertices)[index1] = vertex1;
      (*vertices)[index2] = vertex2;

      (*indices)[index0] = index0;
      (*indices)[index1] = index1;
      (*indices)[index2] = index2;
    }
  }

  ObjectLoader::moveToOrigin(vertices);
  ObjectLoader::translateObject(vertices, offsetX, offsetY, offsetZ);

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
    std::vector<std::string> tokens;

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
      splitText(buffer, ' ', tokens);

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
      splitText(buffer, ' ', tokens);

      const int offset = 3 * iElement;

      (*elements)[offset + 0] = std::stoi(tokens[0]);
      (*elements)[offset + 1] = std::stoi(tokens[1]);
      (*elements)[offset + 2] = std::stoi(tokens[2]);
    }
    LOG_INFO("Reading elements done.");

    // =========================================================================================
    // Calc vertex normal
    // =========================================================================================
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

    veci_pt belongTo = std::make_shared<std::vector<int>>();
    belongTo->resize(cumsum);

    veci_pt tmpCumsumCountNodes = std::make_shared<std::vector<int>>();
    tmpCumsumCountNodes->resize(nVertices);

    for (int iNode = 0; iNode < nVertices; ++iNode) {
      (*tmpCumsumCountNodes)[iNode] = 0;
    }

    for (int iElement = 0; iElement < nElements; ++iElement) {
      const int offset = 3 * iElement;

      const int nodeId0 = (*elements)[offset + 0];
      const int nodeId1 = (*elements)[offset + 1];
      const int nodeId2 = (*elements)[offset + 2];

      const int offset0 = (*cumsumCountNodes)[nodeId0] + (*tmpCumsumCountNodes)[nodeId0];
      (*belongTo)[offset0] = iElement;
      (*tmpCumsumCountNodes)[nodeId0] = (*tmpCumsumCountNodes)[nodeId0] + 1;

      const int offset1 = (*cumsumCountNodes)[nodeId1] + (*tmpCumsumCountNodes)[nodeId1];
      (*belongTo)[offset1] = iElement;
      (*tmpCumsumCountNodes)[nodeId1] = (*tmpCumsumCountNodes)[nodeId1] + 1;

      const int offset2 = (*cumsumCountNodes)[nodeId2] + (*tmpCumsumCountNodes)[nodeId2];
      (*belongTo)[offset2] = iElement;
      (*tmpCumsumCountNodes)[nodeId2] = (*tmpCumsumCountNodes)[nodeId2] + 1;
    }

    // TODO

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

      const Vertex vertex0(vertexCoord0,
                           glm::vec3(1.0f),
                           glm::vec3(0.0f),
                           BARY_CENTER[0],
                           glm::vec2(0.0f),
                           0.0f);
      const Vertex vertex1(vertexCoord1,
                           glm::vec3(1.0f),
                           glm::vec3(0.0f),
                           BARY_CENTER[1],
                           glm::vec2(0.0f),
                           0.0f);
      const Vertex vertex2(vertexCoord2,
                           glm::vec3(1.0f),
                           glm::vec3(0.0f),
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

  ObjectLoader::moveToOrigin(vertices);
  ObjectLoader::translateObject(vertices, offsetX, offsetY, offsetZ);

  LOG_INFO("Num of vertices : " + std::to_string(vertices->size()));
  LOG_INFO("Num of triangles: " + std::to_string(vertices->size() / 3));
}

void ObjectLoader::readLazFile(const std::string &filePath,
                               VertexArray_t vertices,
                               IndexArray_t indices,
                               const float offsetX,
                               const float offsetY,
                               const float offsetZ) {
  // std::ifstream ifs(filePath, std::ios::in | std::ios::binary);

  // if (!ifs.good()) {
  //   std::cerr << "Could not open file for reading: " << filePath << std::endl;
  //   std::cerr << strerror(errno) << std::endl;
  //   return;
  // }

  // liblas::ReaderFactory f;
  // liblas::Reader reader = f.CreateWithStream(ifs);

  // liblas::Header const &header = reader.GetHeader();

  // std::cout << "Compressed: " << ((header.Compressed() == true) ? "true" : "false") << std::endl;
  // std::cout << "Signature: " << header.GetFileSignature() << std::endl;
  // std::cout << "Points count: " << header.GetPointRecordsCount() << std::endl;

  // glm::vec3 min(0.0f);
  // glm::vec3 max(0.0f);

  // while (reader.ReadNextPoint()) {
  //   const liblas::Point &point = reader.GetPoint();
  //   const liblas::Color &col = point.GetColor();

  //   const glm::vec3 position(point.GetX(), point.GetY(), point.GetZ());
  //   const glm::vec3 normal(0.0f);
  //   const glm::vec3 color(col.GetRed(), col.GetGreen(), col.GetBlue());
  //   const glm::vec2 texcoord(0.0f);

  //   // const Vertex vertex(position, color, normal, texcoord, 0.0f);
  //   // vertices->push_back(vertex);

  //   if (position.x < min.x) {
  //     min.x = position.x;
  //   }
  //   if (position.y < min.y) {
  //     min.y = position.y;
  //   }
  //   if (position.z < min.z) {
  //     min.z = position.z;
  //   }
  //   if (position.x > max.x) {
  //     max.x = position.x;
  //   }
  //   if (position.y > max.y) {
  //     max.y = position.y;
  //   }
  //   if (position.z > max.z) {
  //     max.z = position.z;
  //   }
  // }
}

void ObjectLoader::readObjFileWithMaterialGroup(const std::string &filePath,
                                                MaterialGroups_t materialGroups,
                                                const glm::vec3 offset,
                                                const glm::vec3 scale) {
  unsigned int nMaterials = 0;
  unsigned int nFaces = 0;
  unsigned int nVertices = 0;

#if defined(USE_ASSIMP)
  Assimp::Importer importer;
  unsigned int flag = 0;
  flag |= aiProcess_Triangulate;
  // flag |= aiProcess_PreTransformVertices;
  flag |= aiProcess_CalcTangentSpace;
  // flag |= aiProcess_GenSmoothNormals;
  // flag |= aiProcess_GenUVCoords;
  flag |= aiProcess_RemoveRedundantMaterials;
  // flag |= aiProcess_OptimizeMeshes;

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
    size_t indexOffset = 0;  // インデントのオフセット

    for (size_t iFace = 0; iFace < shape.mesh.num_face_vertices.size(); iFace++) {
      nFaces++;

      const int nVertices = shape.mesh.num_face_vertices[iFace];
      const int materialID = isFoundMaterials ? shape.mesh.material_ids[iFace] : 0;

      for (size_t iVertex = 0; iVertex < nVertices; iVertex++) {
        nVertices++;

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

  // Scale and Translate
  VertexArray_t allVertices = std::make_shared<std::vector<Vertex>>();
  IndexArray_t allIndices = std::make_shared<std::vector<uint32_t>>();
  for (const auto &materialGroup : *materialGroups) {
    mergeVertices(materialGroup->vertices, materialGroup->indices, allVertices, allIndices);
  }

  glm::vec3 minCoord, maxCoord;
  std::tie(minCoord, maxCoord) = getCorners(allVertices);
  glm::vec3 centerCoord = (minCoord + maxCoord) / 2.0f;

  // To origin
  for (auto &materialGroup : *materialGroups) {
    ObjectLoader::translateObject(materialGroup->vertices, -centerCoord);
  }

  // Scale
  for (auto &materialGroup : *materialGroups) {
    ObjectLoader::scaleObject(materialGroup->vertices, scale);
  }

  // Translate
  for (auto &materialGroup : *materialGroups) {
    ObjectLoader::translateObject(materialGroup->vertices, offset);
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

  for (int i = 0; i < vertices->size(); i++) {
    (*vertices)[i].position = ((*vertices)[i].position - center) * scale + center;
  }

  glm::vec3 afterMaxCoords(0.0f);
  glm::vec3 afterMinCoords(0.0f);

  for (int i = 0; i < vertices->size(); i++) {
    const Vertex &vertex = (*vertices)[i];
    for (int direction = 0; direction < 3; direction++) {
      if (vertex.position[direction] > afterMaxCoords[direction]) {
        afterMaxCoords[direction] = vertex.position[direction];
      }
      if (vertex.position[direction] < afterMinCoords[direction]) {
        afterMinCoords[direction] = vertex.position[direction];
      }
    }
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

  for (int i = 0; i < vertices->size(); i++) {
    (*vertices)[i].position = (*vertices)[i].position - center;
  }

  glm::vec3 afterMaxCoords(0.0f);
  glm::vec3 afterMinCoords(0.0f);

  for (int i = 0; i < vertices->size(); i++) {
    const Vertex &vertex = (*vertices)[i];
    for (int direction = 0; direction < 3; direction++) {
      if (vertex.position[direction] > afterMaxCoords[direction]) {
        afterMaxCoords[direction] = vertex.position[direction];
      }
      if (vertex.position[direction] < afterMinCoords[direction]) {
        afterMinCoords[direction] = vertex.position[direction];
      }
    }
  }
}

void ObjectLoader::translateObject(VertexArray_t vertices,
                                   const float offsetX,
                                   const float offsetY,
                                   const float offsetZ) {
  glm::vec3 offset(offsetX, offsetY, offsetZ);

  for (int i = 0; i < vertices->size(); i++) {
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

  for (int i = 0; i < vertices->size(); i++) {
    const Vertex &vertex = (*vertices)[i];

    for (int direction = 0; direction < 3; direction++) {
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

void ObjectLoader::splitText(const std::string &s,
                             const char delim,
                             std::vector<std::string> &tokens) {
  tokens.clear();
  std::stringstream ss(s);
  std::string item;

  while (getline(ss, item, delim)) {
    if (!item.empty() && item != "null") {
      tokens.push_back(item);
    }
  }
}

}  // namespace util
}  // namespace simview