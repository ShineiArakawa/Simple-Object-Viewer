#pragma once

#include <Model/DataStructure.hpp>
#include <OpenGL.hpp>
#include <Util/FileUtil.hpp>
#include <Util/Logging.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#define USE_ASSIMP

#if defined(USE_ASSIMP)
#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure

#include <assimp/Importer.hpp>  // C++ importer interface
#endif

namespace simview {
namespace util {
class ObjectLoader {
 private:
  // Nothing

 public:
  static void readFromFile(const std::string& filePath,
                           std::shared_ptr<std::vector<model::Vertex>> vertices,
                           std::shared_ptr<std::vector<uint32_t>> indices,
                           const float offsetX = 0.0f,
                           const float offsetY = 0.0f,
                           const float offsetZ = 0.0f);
  static void readObjFile(const std::string& filePath,
                          std::shared_ptr<std::vector<model::Vertex>> vertices,
                          std::shared_ptr<std::vector<uint32_t>> indices,
                          const float offsetX = 0.0f,
                          const float offsetY = 0.0f,
                          const float offsetZ = 0.0f);
  static void readLazFile(const std::string& filePath,
                          std::shared_ptr<std::vector<model::Vertex>> vertices,
                          std::shared_ptr<std::vector<uint32_t>> indices,
                          const float offsetX = 0.0f,
                          const float offsetY = 0.0f,
                          const float offsetZ = 0.0f);
  static void readObjFileWithMaterialGroup(const std::string& filePath,
                                           model::MaterialGroups_t materialGroups,
                                           const glm::vec3 offset,
                                           const glm::vec3 scale);
  static void scaleObject(std::shared_ptr<std::vector<model::Vertex>> vertices,
                          const float scale);
  static void scaleObject(std::shared_ptr<std::vector<model::Vertex>> vertices,
                          const float scaleX,
                          const float scaleY,
                          const float scaleZ);
  static void scaleObject(std::shared_ptr<std::vector<model::Vertex>> vertices,
                          const glm::vec3 scale);
  static void moveToOrigin(std::shared_ptr<std::vector<model::Vertex>> vertices);
  static void translateObject(std::shared_ptr<std::vector<model::Vertex>> vertices,
                              const float offsetX = 0.0f,
                              const float offsetY = 0.0f,
                              const float offsetZ = 0.0f);
  static void translateObject(std::shared_ptr<std::vector<model::Vertex>> vertices,
                              const glm::vec3 offset);
  static void rotateObject(std::shared_ptr<std::vector<model::Vertex>> vertices,
                           const float angle,
                           const glm::vec3 axis);
  static void mergeVertices(const std::shared_ptr<std::vector<model::Vertex>>& sourceVertices,
                            const std::shared_ptr<std::vector<unsigned int>>& sourceIndices,
                            std::shared_ptr<std::vector<model::Vertex>>& distVertices,
                            std::shared_ptr<std::vector<unsigned int>>& distIndices);
  static std::pair<glm::vec3, glm::vec3> getCorners(std::shared_ptr<std::vector<model::Vertex>> vertices);
};
}  // namespace util
}  // namespace simview