#pragma once

#include <SimView/Model/Primitives.hpp>
#include <SimView/Util/ObjectLoader.hpp>

namespace simview {
namespace model {
class Sphere : public Primitive {
 private:
  float _offsetX;
  float _offsetY;
  float _offsetZ;
  float _scaleX;
  float _scaleY;
  float _scaleZ;
  glm::vec3 _color;
  int _nDivs;

  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_SPHERE = "Sphere";
  inline static const std::string KEY_MODEL_SPHERE_CENTER = "Center";
  inline static const std::string KEY_MODEL_SPHERE_SCALE = "Scale";
  inline static const std::string KEY_MODEL_NUM_DIVS = "nDivs";

 private:
  // nothing
 protected:
  // nothing
 public:
  Sphere(const int nDivs,
         const float offsetX,
         const float offsetY,
         const float offsetZ,
         const float scaleX,
         const float scaleY,
         const float scaleZ,
         const glm::vec3 color);
  ~Sphere();

  void update() override{};
  void initVAO() override;
  void paintGL(
      const TransformationContext& transCtx,  // transCtx
      const LightingContext& lightingCtx,     // lightingCtx
      const RenderingContext& renderingCtx    // renderingCtx
      ) override;
  void drawGL(const int& index = 0) override;
  void drawAllGL(const glm::mat4& lightMvpMat) override;

  std::string getObjectType() override { return KEY_MODEL_SPHERE; };

  static void createSphere(const int nDivs,
                           VertexArray_t vertices,
                           IndexArray_t indices,
                           const bool isDoubled = false);
  static void createSphere(const int nDivs,
                           const glm::vec3 color,
                           VertexArray_t vertices,
                           IndexArray_t indices,
                           const bool isDoubled = false);
};

using Sphere_t = std::shared_ptr<Sphere>;

}  // namespace model
}  // namespace simview