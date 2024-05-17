#pragma once

#include <SimView/Model/Primitives.hpp>
#include <SimView/Util/ObjectLoader.hpp>

namespace simview {
namespace model {

class LineSet : public Primitive {
 private:
  std::shared_ptr<std::vector<vec3f_t>> _positions;
  float _lineWidth;
  glm::vec3 _color;

  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_LINE_SET = "Line set";

 private:
  // nothing
 protected:
  // nothing
 public:
  LineSet(const std::shared_ptr<std::vector<vec3f_t>> positions,
          const glm::vec3 color,
          const float lineWidth);
  ~LineSet();

  void update() override{};
  void initVAO() override;
  void paintGL(
      const TransformationContext& transCtx,  // transCtx
      const LightingContext& lightingCtx,     // lightingCtx
      const RenderingContext& renderingCtx    // renderingCtx
      ) override;
  void drawGL(const int& index = 0) override;
  void drawAllGL(const glm::mat4& lightMvpMat) override;

  std::string getObjectType() override { return KEY_MODEL_LINE_SET; };
};

using LineSet_t = std::shared_ptr<LineSet>;

}  // namespace model
}  // namespace simview