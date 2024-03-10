#include <Renderer/Renderer.hpp>
#include <iostream>

namespace simview {
namespace renderer {

using namespace model;

Renderer::Renderer(const int* windowWidth,
                   const int* windowHeight,
                   Model_t model,
                   const bool renderToFrameBuffer) {
  _windowWidth = windowWidth;
  _windowHeight = windowHeight;
  _model = model;

  if (renderToFrameBuffer) {
    _frameBuffer = std::make_shared<FrameBuffer>(*_windowWidth, *_windowHeight);
  }

  _depthRenderer = std::make_shared<DepthRenderer>(_model->getDepthShader());
}

Renderer::~Renderer() {}

void Renderer::initModelMatrices() {
  _projMat = glm::perspective(
      glm::radians(45.0f),                           // fovy
      (float)*_windowWidth / (float)*_windowHeight,  // aspect
      0.1f,                                          // near
      1000.0f                                        // far
  );
  _acRotMat = glm::mat4(1.0);
  _acTransMat = glm::mat4(1.0);
  _acScaleMat = glm::mat4(1.0);
}

void Renderer::initLightMatrices() {
  _lightTrasMat = glm::mat4(1.0);
  _lightProjMat = glm::perspective(
      glm::radians(90.0f),                                                               // fovy
      (float)_depthRenderer->DEPTH_MAP_WIDTH / (float)_depthRenderer->DEPTH_MAP_HEIGHT,  // aspect
      0.1f,                                                                              // near
      1000.0f                                                                            // far
  );
}

void Renderer::initializeGL() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  auto RGBA = _model->getBackgroundColor();
  glClearColor(RGBA[0], RGBA[1], RGBA[2], RGBA[3]);

  initModelMatrices();
  initLightMatrices();
}

void Renderer::paintGL(const bool& renderShadowMap) {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  const glm::mat4& modelMat = _acTransMat * _acRotMat * _acScaleMat;
  const glm::mat4& lightMvpMat = _lightProjMat * getLightViewMat(modelMat) * modelMat;

  // ====================================================================
  // Render depth map for shadow maping
  // ====================================================================
  if (renderShadowMap) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, _depthRenderer->DEPTH_MAP_WIDTH, _depthRenderer->DEPTH_MAP_HEIGHT);

    {
      _depthRenderer->bind();
      glDepthFunc(GL_LESS);

      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);

      _model->drawGL(lightMvpMat);

      glDisable(GL_CULL_FACE);
      _depthRenderer->unbind();
    }
  }

  // ====================================================================
  // Render scene
  // ====================================================================
  if (_frameBuffer != nullptr) {
    _frameBuffer->bind();
  }

  glViewport(0, 0, *_windowWidth, *_windowHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  const glm::vec4& RGBA = _model->getBackgroundColor();
  glClearColor(RGBA[0], RGBA[1], RGBA[2], RGBA[3]);

  const glm::mat4& mvMat = _viewMat * modelMat;
  const glm::mat4& mvpMat = _projMat * mvMat;
  const glm::mat4& lightMat = mvMat * _lightTrasMat;

  {
    _model->paintGL(
        mvMat,                           // mvMat
        mvpMat,                          // mvpMat
        lightMat,                        // lightMat
        lightMvpMat,                     // lightMvpMat
        _depthRenderer->getDepthMapId()  // depthMapId
    );
  }

  if (_frameBuffer != nullptr) {
    _frameBuffer->unbind();
  }

  // ====================================================================
  // Update time state
  // ====================================================================
  _model->tick(TICK_VALUE);
}

void Renderer::resizeGL() {
  const float fWindowWidth = (float)*_windowWidth;
  const float fWindowHeight = (float)*_windowHeight;

  // Update projection matrix
  _projMat = glm::perspective(
      glm::radians(45.0f),           // fovy
      fWindowWidth / fWindowHeight,  // aspect
      0.1f,                          // near
      1000.0f                        // far
  );

  // Update frame buffer size
  if (_frameBuffer != nullptr) {
    _frameBuffer->rescaleFrameBuffer(fWindowWidth, fWindowHeight);
  }
}

FrameBuffer_t Renderer::getFrameBuffer() {
  return _frameBuffer;
}

DepthRenderer_t Renderer::getDepthRenderer() {
  return _depthRenderer;
}

void Renderer::updateScale(const float acScale) {
  _acScaleMat = glm::scale(glm::vec3(acScale, acScale, acScale));
}

void Renderer::updateTranslate(const glm::vec4& newPosScreenSpace, const glm::vec4& oldPosScreenSpace) {
  const glm::mat4& invMvpMat = glm::inverse(_projMat * _viewMat);

  glm::vec4 newPosObjSpace = invMvpMat * newPosScreenSpace;
  glm::vec4 oldPosObjSpace = invMvpMat * oldPosScreenSpace;
  newPosObjSpace /= newPosObjSpace.w;
  oldPosObjSpace /= oldPosObjSpace.w;

  const glm::vec3& transWorldSpace = glm::vec3(newPosObjSpace - oldPosObjSpace);

  _acTransMat = glm::translate(transWorldSpace) * _acTransMat;
}

void Renderer::updateRotate(const glm::vec3& u, const glm::vec3& v) {
  const double& angle = std::acos(std::max(-1.0f, std::min(glm::dot(u, v), 1.0f)));
  const glm::vec3& rotAxis = glm::cross(u, v);
  const glm::mat4& c2wMat = glm::inverse(_viewMat);
  const glm::vec3& rotAxisWorldSpace = glm::vec3(c2wMat * glm::vec4(rotAxis, 0.0f));
  rotateModel((float)(1.0 * angle), rotAxisWorldSpace);
}

void Renderer::rotateModel(const float angle, const glm::vec3& rotAxisWorldSpace) {
  _acRotMat = glm::rotate(angle, rotAxisWorldSpace) * _acRotMat;
}

void Renderer::rotateLight(const float angle, const glm::vec3& rotAxisWorldSpace) {
  _lightTrasMat = glm::rotate(angle, rotAxisWorldSpace) * _lightTrasMat;
}

void Renderer::setViewMat(const glm::mat4& viewMat) {
  _viewMat = viewMat;
}

glm::vec4 Renderer::getOriginScreenSpace() {
  return (_projMat * _viewMat) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

glm::vec3 Renderer::getLightPosInWorldSpace() {
  return (_lightTrasMat * _model->getLightPos()).xyz();
}

glm::mat4 Renderer::getLightViewMat(const glm::mat4& modelMat) {
  return glm::lookAt(
      (modelMat * _lightTrasMat * _model->getLightPos()).xyz(),  // eye
      glm::vec3(0.0f, 0.0f, 0.0f),                               // center
      glm::vec3(0.0f, 0.0f, 1.0f)                                // up
  );
}
}  // namespace renderer
}  // namespace simview