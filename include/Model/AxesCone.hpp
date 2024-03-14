#pragma once

#include <Model/Primitives.hpp>
#include <Util/ObjectLoader.hpp>

namespace simview {
namespace model {

class AxesCone : public Primitive {
 private:
  float _offsetX;
  float _offsetY;
  float _offsetZ;
  float _scale;
  glm::vec3 _color;
  int _nDivs;

  int _indexBufferSize;
  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_AXES_CONE = "Axes Cone";

  inline static const float CONE_HIGHT = 0.2f;
  inline static const float CONE_RADIUS = 0.2f;
  inline static const float CYLINDER_RADIUS = 0.1f;

  inline static const glm::vec4 POSITION_IN_DEVICE_SPACE = glm::vec4(-0.8f, -0.8f, 0.99f, 1.0f);

 private:
  // nothing
 protected:
  // nothing
 public:
  AxesCone(const int nDivs, const float offsetX, const float offsetY, const float offsetZ, const float scale);
  ~AxesCone();
  void update() override{};
  void initVAO() override;
  void paintGL(
      const TransformationContext& transCtx,  // transCtx
      const LightingContext& lightingCtx,     // lightingCtx
      const RenderingContext& renderingCtx    // renderingCtx
      ) override;
  void drawGL(const int& index = 0) override;
  void drawAllGL(const glm::mat4& lightMvpMat) override;

  std::string getObjectType() override { return KEY_MODEL_AXES_CONE; };
  void createXorientedCone(
      VertexArray_t& vertices,
      IndexArray_t& indices,
      const glm::vec3& color) const;
};

using AxesCone_t = std::shared_ptr<AxesCone>;

}  // namespace model
}  // namespace simview