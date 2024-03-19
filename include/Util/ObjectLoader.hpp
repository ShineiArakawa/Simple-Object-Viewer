#pragma once

#include <OpenGL.hpp>
#include <Util/DataStructure.hpp>
#include <Util/FileUtil.hpp>
#include <Util/Geometry.hpp>
#include <Util/Logging.hpp>
#include <Util/StringUtil.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Include assimp
#if defined(SIMVIEW_USE_ASSIMP)
#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure

#include <assimp/Importer.hpp>  // C++ importer interface
#endif

// Include VTK
#if defined(SIMVIEW_WITH_VTK)
#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkCellType.h"
#include "vtkCellTypes.h"
#include "vtkCommonCoreModule.h"
#include "vtkPoints.h"
#include "vtkSmartPointer.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnstructuredGrid.h"
#include "vtkXMLUnstructuredGridReader.h"
#endif

namespace simview {
namespace util {

class ObjectLoader {
 private:
#if defined(SIMVIEW_WITH_VTK)
  inline static const std::array<vec3i_t, 1> VTK_TRIANGLE_IDS_TRIANGLE = {{{{0, 1, 2}}}};
  inline static const std::array<vec3i_t, 2> VTK_TRIANGLE_IDS_QUAD = {{{{0, 1, 2}},
                                                                       {{2, 3, 0}}}};
  inline static const std::array<vec3i_t, 4> VTK_TRIANGLE_IDS_TETRA = {{{{0, 1, 2}},
                                                                        {{1, 2, 3}},
                                                                        {{2, 3, 0}},
                                                                        {{3, 0, 1}}}};
  inline static const std::array<vec3i_t, 12> VTK_TRIANGLE_IDS_HEXAHEDRON = {{{{0, 1, 2}},
                                                                              {{2, 3, 0}},
                                                                              {{1, 5, 6}},
                                                                              {{6, 2, 1}},
                                                                              {{5, 4, 7}},
                                                                              {{7, 6, 5}},
                                                                              {{4, 0, 3}},
                                                                              {{3, 7, 4}},
                                                                              {{3, 2, 6}},
                                                                              {{6, 7, 3}},
                                                                              {{1, 0, 4}},
                                                                              {{4, 5, 1}}}};
  inline static const std::array<vec3i_t, 8> VTK_TRIANGLE_IDS_WEDGE = {{{{2, 1, 0}},
                                                                        {{5, 3, 4}},
                                                                        {{1, 4, 3}},
                                                                        {{3, 0, 1}},
                                                                        {{0, 3, 5}},
                                                                        {{5, 2, 0}},
                                                                        {{1, 2, 5}},
                                                                        {{5, 4, 1}}}};
#endif

 public:
  static std::vector<std::string> getReadableExtensionList();
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
  static void readPchFile(const std::string& filePath,
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
  static void readVtkFile(const std::string& filePath,
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
};  // namespace util

}  // namespace util
}  // namespace simview
