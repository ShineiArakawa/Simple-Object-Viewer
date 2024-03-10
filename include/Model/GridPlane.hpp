#pragma once

#include <Model/Primitives.hpp>

namespace simview {
namespace model {

class GridPlane : public Primitive {
 private:
  int _nDivs;
  glm::vec2 _minCoords;
  glm::vec2 _maxCoords;
  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;
  int _indexBufferSize;

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_GRID_PLANE = "Grid plane";

 private:
  // nothing
 protected:
  // nothing
 public:
  GridPlane(const int& nDivs,
            const glm::vec2& minCoords,
            const glm::vec2& maxCoords);
  ~GridPlane();

  void update() override{};
  void initVAO() override;
  void paintGL(const glm::mat4& mvMat,
               const glm::mat4& mvpMat,
               const glm::mat4& lightMat,
               const glm::vec3& lightPos,
               const float& shininess,
               const float& ambientIntensity,
               const glm::vec3& wireFrameColor,
               const float& wireFrameWidth,
               const GLuint& depthTextureId,
               const glm::mat4& lightMvpMat) override;
  void drawGL(const int& index = 0) override;
  void drawAllGL(const glm::mat4& lightMvpMat) override;

  std::string getObjectType() override { return KEY_MODEL_GRID_PLANE; };
};

using GridPlane_t = std::shared_ptr<GridPlane>;

}  // namespace model
}  // namespace simview