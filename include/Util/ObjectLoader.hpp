#pragma once

#include <OpenGL.hpp>
#include <Util/DataStructure.hpp>
#include <Util/FileUtil.hpp>
#include <Util/Geometry.hpp>
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
                           VertexArray_t vertices,
                           IndexArray_t indices,
                           const float offsetX = 0.0f,
                           const float offsetY = 0.0f,
                           const float offsetZ = 0.0f);
  static void readObjFile(const std::string& filePath,
                          VertexArray_t vertices,
                          IndexArray_t indices,
                          const float offsetX = 0.0f,
                          const float offsetY = 0.0f,
                          const float offsetZ = 0.0f);
  static void readMshFile(const std::string& filePath,
                          VertexArray_t vertices,
                          IndexArray_t indices,
                          const float offsetX = 0.0f,
                          const float offsetY = 0.0f,
                          const float offsetZ = 0.0f);
  static void readLazFile(const std::string& filePath,
                          VertexArray_t vertices,
                          IndexArray_t indices,
                          const float offsetX = 0.0f,
                          const float offsetY = 0.0f,
                          const float offsetZ = 0.0f);
  static void readObjFileWithMaterialGroup(const std::string& filePath,
                                           MaterialGroups_t materialGroups,
                                           const glm::vec3 offset,
                                           const glm::vec3 scale);
  static void scaleObject(VertexArray_t vertices,
                          const float scale);
  static void scaleObject(VertexArray_t vertices,
                          const float scaleX,
                          const float scaleY,
                          const float scaleZ);
  static void scaleObject(VertexArray_t vertices,
                          const glm::vec3 scale);
  static void moveToOrigin(VertexArray_t vertices);
  static void translateObject(VertexArray_t vertices,
                              const float offsetX = 0.0f,
                              const float offsetY = 0.0f,
                              const float offsetZ = 0.0f);
  static void translateObject(VertexArray_t vertices,
                              const glm::vec3 offset);
  static void rotateObject(VertexArray_t vertices,
                           const float angle,
                           const glm::vec3 axis);
  static void mergeVertices(const VertexArray_t& sourceVertices,
                            const IndexArray_t& sourceIndices,
                            VertexArray_t& distVertices,
                            IndexArray_t& distIndices);
  static std::pair<glm::vec3, glm::vec3> getCorners(VertexArray_t vertices);
  static void ObjectLoader::splitText(const std::string& s,
                                      const char delim,
                                      std::vector<std::string>& tokens);
};

}  // namespace util
}  // namespace simview