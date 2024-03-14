#pragma once

#include <Model/Model.hpp>
#include <Model/Primitives.hpp>
#include <Model/RenderingContext.hpp>
#include <OpenGL.hpp>
#include <Renderer/DepthRenderer.hpp>
#include <Renderer/FrameBuffer.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace simview {
namespace renderer {

class Renderer {
 private:
  inline static const float TICK_VALUE = 1.0f;

  // Model
  model::Model_t _model;
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

  FrameBuffer_t _frameBuffer = nullptr;
  DepthRenderer_t _depthRenderer = nullptr;

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
           model::Model_t models,
           const bool renderToFrameBuffer = false);
  ~Renderer();

  void initModelMatrices();
  void initLightMatrices();
  void initializeGL();

  void paintGL(const bool& renderShadowMap = true);
  void resizeGL();

  FrameBuffer_t getFrameBuffer();
  DepthRenderer_t getDepthRenderer();

  void updateScale(const float);
  void updateTranslate(const glm::vec4& newPosScreenSpace,
                       const glm::vec4& oldPosScreenSpace);
  void updateRotate(const glm::vec3& u,
                    const glm::vec3& v);

  void rotateModel(const float angle, const glm::vec3&);
  void rotateLight(const float angle, const glm::vec3&);
  void setViewMat(const glm::mat4&);

  glm::vec4 getOriginScreenSpace();
  glm::vec3 getLightPosInWorldSpace();
  glm::mat4 getLightViewMat(const glm::mat4& modelMat);
};

using Renderer_t = std::shared_ptr<Renderer>;

}  // namespace renderer
}  // namespace simview