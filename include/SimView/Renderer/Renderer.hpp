#pragma once

#include <SimView/Model/Model.hpp>
#include <SimView/Model/Primitives.hpp>
#include <SimView/Model/RenderingContext.hpp>
#include <SimView/OpenGL.hpp>
#include <SimView/Renderer/DepthRenderer.hpp>
#include <SimView/Renderer/FrameBuffer.hpp>
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

  // Matrices
  glm::mat4 _viewMat;
  glm::mat4 _projMat;
  glm::mat4 _acRotMat;
  glm::mat4 _acTransMat;
  glm::mat4 _acScaleMat;

  // Light
  glm::mat4 _lightTrasMat;
  glm::mat4 _lightProjMat;

  // Model
  model::Model_t _model;

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

  glm::mat4 perspective(const float& fovyInDegrees,
                        const float& aspectRatio,
                        const float& nearPlane,
                        const float& rearPlane);
};

using Renderer_t = std::shared_ptr<Renderer>;

}  // namespace renderer
}  // namespace simview