#pragma once

#include <SimView/Model/Primitives.hpp>

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
  void paintGL(
      const TransformationContext& transCtx,  // transCtx
      const LightingContext& lightingCtx,     // lightingCtx
      const RenderingContext& renderingCtx    // renderingCtx
      ) override;
  void drawGL(const int& index = 0) override;
  void drawAllGL(const glm::mat4& lightMvpMat) override;

  std::string getObjectType() override { return KEY_MODEL_GRID_PLANE; };
};

using GridPlane_t = std::shared_ptr<GridPlane>;

}  // namespace model
}  // namespace simview