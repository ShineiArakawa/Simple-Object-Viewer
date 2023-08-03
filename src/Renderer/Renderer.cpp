#include <Renderer/Renderer.hpp>
#include <iostream>

Renderer::Renderer(int* windowWidth, int* windowHeight, std::shared_ptr<Model> models) {
  _windowWidth = windowWidth;
  _windowHeight = windowHeight;
  _models = models;
}

Renderer::~Renderer() {}

void Renderer::initVAO() { _models->initVAO(); }

void Renderer::initializeGL() {
  // Enable depth testing
  glEnable(GL_DEPTH_TEST);

  // Background color (black)
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  // glClearColor(6.0f / 255.0f, 230.0f / 255.0f, 230.0f / 255.0f, 1.0f);

  // Initialize VAO
  initVAO();

  // Initialize transformation matrices for camera pose
  _projMat = glm::perspective(glm::radians(45.0f), (float)*_windowWidth / (float)*_windowHeight, 0.1f, 1000.0f);

  // Initialize transformation matrices for arcball control
  _acRotMat = glm::mat4(1.0);
  _acTransMat = glm::mat4(1.0);
  _acScaleMat = glm::mat4(1.0);
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
  // Transformation from screen space to world space (= inverse of MVP matrix)
  glm::mat4 invMvpMat = glm::inverse(_projMat * _viewMat);

  // Transform screen-space positions to world-space positions
  glm::vec4 newPosObjSpace = invMvpMat * newPosScreenSpace;
  glm::vec4 oldPosObjSpace = invMvpMat * oldPosScreenSpace;
  newPosObjSpace /= newPosObjSpace.w;
  oldPosObjSpace /= oldPosObjSpace.w;

  // Calculate the amount of translation in world space
  const glm::vec3 transWorldSpace = glm::vec3(newPosObjSpace - oldPosObjSpace);

  // Calculate translation matrix
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
