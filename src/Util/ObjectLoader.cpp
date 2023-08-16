#include <Util/ObjectLoader.hpp>

#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#endif

void ObjectLoader::readObjFile(const std::string &filePath, std::shared_ptr<std::vector<Vertex>> vertices,
                               std::shared_ptr<std::vector<uint32_t>> indices, const float offsetX, const float offsetY, const float offsetZ) {
  std::cout << "### Loaded obj file: " << filePath << std::endl;

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;

  bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str());

  if (!err.empty()) {
    std::cerr << "[WARNING] " << err << std::endl;
  }

  if (!success) {
    std::cerr << "Failed to load OBJ file: " << filePath << std::endl;
    exit(1);
  }

  // Vertex配列の作成
  // Create vertex array

  glm::vec3 maxCoords(0.0f);
  glm::vec3 minCoords(0.0f);

  for (int s = 0; s < shapes.size(); s++) {
    const tinyobj::mesh_t &mesh = shapes[s].mesh;

    int nVertices = mesh.indices.size();
    // std::array<std::vector<int>, nVertices> verticesArray;

    for (int i = 0; i < nVertices; i++) {
      const tinyobj::index_t &index = mesh.indices[i];

      glm::vec3 position, normal;
      glm::vec2 texcoord;

      if (index.vertex_index >= 0) {
        position = glm::vec3(attrib.vertices[index.vertex_index * 3 + 0], attrib.vertices[index.vertex_index * 3 + 1],
                             attrib.vertices[index.vertex_index * 3 + 2]);

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
        normal = glm::vec3(attrib.normals[index.normal_index * 3 + 0], attrib.normals[index.normal_index * 3 + 1],
                           attrib.normals[index.normal_index * 3 + 2]);
      }

      if (index.texcoord_index >= 0) {
        texcoord = glm::vec2(attrib.texcoords[index.texcoord_index * 2 + 0], attrib.texcoords[index.texcoord_index * 2 + 1]);
      }

      const Vertex vertex(position, normal, normal, texcoord, 0.0f);

      indices->push_back((uint32_t)vertices->size());
      vertices->push_back(vertex);
    }
  }

  // std::cout << "(x_min, y_min, z_min) = (" << minCoords.x << ", " << minCoords.y << ", " << minCoords.z << ")" << std::endl;
  // std::cout << "(x_max, y_max, z_max) = (" << maxCoords.x << ", " << maxCoords.y << ", " << maxCoords.z << ")" << std::endl;

  ObjectLoader::moveToOrigin(vertices);
  ObjectLoader::move(vertices, offsetX, offsetY, offsetZ);
}

void ObjectLoader::scaleObject(std::shared_ptr<std::vector<Vertex>> vertices, const float scale) {
  ObjectLoader::scaleObject(vertices, scale, scale, scale);
}

void ObjectLoader::scaleObject(std::shared_ptr<std::vector<Vertex>> vertices, const float scaleX, const float scaleY, const float scaleZ) {
  glm::vec3 maxCoords(0.0f);
  glm::vec3 minCoords(0.0f);

  for (int i = 0; i < vertices->size(); i++) {
    const Vertex &vertex = (*vertices)[i];
    for (int direction = 0; direction < 3; direction++) {
      if (vertex.position[direction] > maxCoords[direction]) {
        maxCoords[direction] = vertex.position[direction];
      }
      if (vertex.position[direction] < minCoords[direction]) {
        minCoords[direction] = vertex.position[direction];
      }
    }
  }

  glm::vec3 center = (maxCoords + minCoords) / 2.0f;
  glm::vec3 range = maxCoords - minCoords;
  glm::vec3 scale(scaleX, scaleY, scaleZ);

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

  std::cout << "### Obj is scaled" << std::endl;
  std::cout << "(x_min, y_min, z_min) = (" << afterMinCoords.x << ", " << afterMinCoords.y << ", " << afterMinCoords.z << ")" << std::endl;
  std::cout << "(x_max, y_max, z_max) = (" << afterMaxCoords.x << ", " << afterMaxCoords.y << ", " << afterMaxCoords.z << ")" << std::endl;
}

void ObjectLoader::moveToOrigin(std::shared_ptr<std::vector<Vertex>> vertices) {
  glm::vec3 maxCoords(0.0f);
  glm::vec3 minCoords(0.0f);

  for (int i = 0; i < vertices->size(); i++) {
    const Vertex &vertex = (*vertices)[i];
    for (int direction = 0; direction < 3; direction++) {
      if (vertex.position[direction] > maxCoords[direction]) {
        maxCoords[direction] = vertex.position[direction];
      }
      if (vertex.position[direction] < minCoords[direction]) {
        minCoords[direction] = vertex.position[direction];
      }
    }
  }

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

  // std::cout << "### Obj is moved to the origin" << std::endl;
  // std::cout << "(x_min, y_min, z_min) = (" << afterMinCoords.x << ", " << afterMinCoords.y << ", " << afterMinCoords.z << ")" << std::endl;
  // std::cout << "(x_max, y_max, z_max) = (" << afterMaxCoords.x << ", " << afterMaxCoords.y << ", " << afterMaxCoords.z << ")" << std::endl;
}

void ObjectLoader::move(std::shared_ptr<std::vector<Vertex>> vertices, const float offsetX, const float offsetY, const float offsetZ) {
  glm::vec3 offset(offsetX, offsetY, offsetZ);

  for (int i = 0; i < vertices->size(); i++) {
    (*vertices)[i].position = (*vertices)[i].position + offset;
  }
}
