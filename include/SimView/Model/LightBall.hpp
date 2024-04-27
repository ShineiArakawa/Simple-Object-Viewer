#pragma once

#include <SimView/Model/Primitives.hpp>
#include <SimView/Model/Sphere.hpp>
#include <SimView/Util/ObjectLoader.hpp>

namespace simview {
namespace model {

class LightBall : public Primitive {
 private:
  inline static const int NUM_DIVISIONS = 20;
  inline static const glm::vec3 COLOR = glm::vec3(250.0f / 255.0f, 180.0f / 255.0f, 0.0f);

  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_LIGHT_BALL = "Light ball";

 private:
  // nothing
 protected:
  // nothing
 public:
  LightBall();
  ~LightBall();

  void update() override{};
  void initVAO() override;
  void paintGL(
      const TransformationContext& transCtx,  // transCtx
      const LightingContext& lightingCtx,     // lightingCtx
      const RenderingContext& renderingCtx    // renderingCtx
      ) override;
  void drawGL(const int& index = 0) override;
  void drawAllGL(const glm::mat4& lightMvpMat) override;

  std::string getObjectType() override { return KEY_MODEL_LIGHT_BALL; };
};

using LightBall_t = std::shared_ptr<LightBall>;

}  // namespace model
}  // namespace simview