#pragma once

#include <Model/Model.hpp>
#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <Renderer/DepthRenderer.hpp>
#include <Renderer/FrameBuffer.hpp>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace oglt {
namespace renderer {
class Renderer {
 public:
  using pFrameBuffer = std::shared_ptr<FrameBuffer>;
  using pDepthRenderer = std::shared_ptr<DepthRenderer>;

 private:
  inline static const float TICK_VALUE = 1.0f;

  // Model
  std::shared_ptr<model::Model> _model;
  glm::mat4 _viewMat;
  glm::mat4 _projMat;
  glm::mat4 _acRotMat;
  glm::mat4 _acTransMat;
  glm::mat4 _acScaleMat;

  // Light
  glm::mat4 _lightTrasMat;
  glm::mat4 _lightProjMat;

  const int* _windowWidth = nullptr;
  const int* _windowHeight = nullptr;

  pFrameBuffer _frameBuffer = nullptr;
  pDepthRenderer _depthRenderer = nullptr;

 protected:
  // nothing
 public:
  // nothing
 private:
  // nothing
 protected:
  // nothing
 public:
  Renderer(const int* windowWidth,
           const int* windowHeight,
           std::shared_ptr<model::Model> models,
           const bool renderToFrameBuffer = false);
  ~Renderer();

  void initModelMatrices();
  void initLightMatrices();
  void initializeGL();

  void paintGL();
  void resizeGL();

  pFrameBuffer getFrameBuffer();
  pDepthRenderer getDepthRenderer();

  void updateScale(const float);
  void updateTranslate(const glm::vec4& newPosScreenSpace, const glm::vec4& oldPosScreenSpace);
  void updateRotate(const glm::vec3& u, const glm::vec3& v);

  void rotateModel(const float angle, const glm::vec3&);
  void rotateLight(const float angle, const glm::vec3&);
  void setViewMat(const glm::mat4&);

  glm::vec4 getOriginScreenSpace();
  glm::vec3 getLightPosInWorldSpace();
  glm::mat4 getLightViewMat(const glm::mat4& modelMat);
};
}  // namespace renderer
}  // namespace oglt