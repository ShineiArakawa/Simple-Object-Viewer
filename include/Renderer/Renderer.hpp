#pragma once
#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <Model/Model.hpp>
#include <Model/Primitives.hpp>
#include <Model/ShaderCompiler.hpp>
#include <OpenGL.hpp>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

class Renderer {
 private:
  inline static const float TICK_VALUE = 1.0f;

  std::shared_ptr<Model> _model;
  glm::mat4 _viewMat;
  glm::mat4 _projMat;
  glm::mat4 _acRotMat;
  glm::mat4 _acTransMat;
  glm::mat4 _acScaleMat;
  glm::mat4 _lightTrasMat;

  const int* _windowWidth = nullptr;
  const int* _windowHeight = nullptr;

 protected:
  // nothing
 public:
  // nothing
 private:
  // nothing
 protected:
  // nothing
 public:
  Renderer(const int* windowWidth, const int* windowHeight, std::shared_ptr<Model> models);
  ~Renderer();

  void initModelMatrices();
  void initLightMatrices();
  void initVAO();
  void initializeGL();

  void paintGL();
  void resizeGL();

  void updateScale(const float);
  void updateTranslate(const glm::vec4& newPosScreenSpace, const glm::vec4& oldPosScreenSpace);
  void updateRotate(const glm::vec3& u, const glm::vec3& v);

  void rotateModel(const float angle, const glm::vec3&);
  void rotateLight(const float angle, const glm::vec3&);
  void setViewMat(const glm::mat4&);
  glm::vec4 getOriginScreenSpace();
};

#endif