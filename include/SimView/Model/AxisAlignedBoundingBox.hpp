#pragma once

#include <SimView/OpenGL.hpp>
#include <SimView/Util/DataStructure.hpp>
#include <SimView/Util/ObjectLoader.hpp>
#include <memory>
#include <string>
#include <vector>

namespace simview {
namespace model {

class AxisAlignedBoundingBox {
 private:
  glm::vec3 _minCoords;
  glm::vec3 _maxCoords;
  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;

  inline static const glm::vec3 POSITIONS[8] = {
      glm::vec3(0.0f, 0.0f, 0.0f),  // 0
      glm::vec3(1.0f, 0.0f, 0.0f),  // 1
      glm::vec3(0.0f, 1.0f, 0.0f),  // 2
      glm::vec3(0.0f, 0.0f, 1.0f),  // 3
      glm::vec3(1.0f, 1.0f, 0.0f),  // 4
      glm::vec3(0.0f, 1.0f, 1.0f),  // 5
      glm::vec3(1.0f, 0.0f, 1.0f),  // 6
      glm::vec3(1.0f, 1.0f, 1.0f)   // 7
  };

  inline static const unsigned int LINES[12][2] = {
      {0, 1},
      {1, 4},
      {4, 2},
      {2, 0},
      {3, 6},
      {6, 7},
      {7, 5},
      {5, 3},
      {0, 3},
      {1, 6},
      {4, 7},
      {2, 5}};

  inline static const glm::vec3 COLOR = glm::vec3(10.0f / 255.0f, 220.0f / 255.0f, 50.0f / 255.0f);

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_BOUNDING_BOX = "Bounding Box";

 private:
  void initVAO();

 protected:
  // nothing
 public:
  AxisAlignedBoundingBox(const glm::vec3& minCoords,
                         const glm::vec3& maxCoords);
  ~AxisAlignedBoundingBox();
  void draw() const;
};

using AxisAlignedBoundingBox_t = std::shared_ptr<AxisAlignedBoundingBox>;

}  // namespace model
}  // namespace simview