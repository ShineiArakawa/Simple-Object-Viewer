#include <Renderer/Renderer.hpp>
#include <iostream>

Renderer::Renderer(int* windowWidth, int* windowHeight, std::shared_ptr<Model> models) {
  _windowWidth = windowWidth;
  _windowHeight = windowHeight;
  _models = models;
}

Renderer::~Renderer() {}

void Renderer::initMatrices() {
  _projMat = glm::perspective(glm::radians(45.0f), (float)*_windowWidth / (float)*_windowHeight, 0.1f, 1000.0f);
  _acRotMat = glm::mat4(1.0);
  _acTransMat = glm::mat4(1.0);
  _acScaleMat = glm::mat4(1.0);
}

void Renderer::initVAO() { _models->initVAO(); }

void Renderer::initializeGL() {
  glEnable(GL_DEPTH_TEST);

  auto RGBA = _models->getBackgroundColor();
  glClearColor(RGBA[0], RGBA[1], RGBA[2], RGBA[3]);

  initMatrices();
  initVAO();
}

void Renderer::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 modelMat = _acTransMat * _acRotMat * _acScaleMat;
  glm::mat4 mvpMat = _projMat * _viewMat * modelMat;

  _models->paintGL(mvpMat);
  _models->tick(TICK_VALUE);
}

void Renderer::updateScale(float acScale) { _acScaleMat = glm::scale(glm::vec3(acScale, acScale, acScale)); }

void Renderer::updateTranslate(const glm::vec4& newPosScreenSpace, const glm::vec4& oldPosScreenSpace) {
  glm::mat4 invMvpMat = glm::inverse(_projMat * _viewMat);

  glm::vec4 newPosObjSpace = invMvpMat * newPosScreenSpace;
  glm::vec4 oldPosObjSpace = invMvpMat * oldPosScreenSpace;
  newPosObjSpace /= newPosObjSpace.w;
  oldPosObjSpace /= oldPosObjSpace.w;

  const glm::vec3 transWorldSpace = glm::vec3(newPosObjSpace - oldPosObjSpace);

  _acTransMat = glm::translate(transWorldSpace) * _acTransMat;
}

void Renderer::updateRotate(const glm::vec3& u, const glm::vec3& v) {
  const double angle = std::acos(std::max(-1.0f, std::min(glm::dot(u, v), 1.0f)));
  const glm::vec3 rotAxis = glm::cross(u, v);
  const glm::mat4 c2wMat = glm::inverse(_viewMat);
  const glm::vec3 rotAxisWorldSpace = glm::vec3(c2wMat * glm::vec4(rotAxis, 0.0f));
  rotateModel((float)(1.0 * angle), rotAxisWorldSpace);
}

void Renderer::resizeGL() { _projMat = glm::perspective(glm::radians(45.0f), (float)*_windowWidth / (float)*_windowHeight, 0.1f, 1000.0f); }

void Renderer::rotateModel(const float angle, const glm::vec3 rotAxisWorldSpace) { _acRotMat = glm::rotate(angle, rotAxisWorldSpace) * _acRotMat; }

void Renderer::setViewMat(glm::mat4 viewMat) { _viewMat = viewMat; }

glm::vec4 Renderer::getOriginScreenSpace() { return (_projMat * _viewMat) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); }
