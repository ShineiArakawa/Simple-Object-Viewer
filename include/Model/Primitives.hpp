#pragma once
#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

#define _USE_MATH_DEFINES
#include <math.h>

#include <OpenGL.hpp>
#include <array>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct Vertex {
  Vertex(){};
  Vertex(const glm::vec3& position_, const glm::vec3& color_, const glm::vec3& normal_, const glm::vec2& uv_, const float id_)
      : position(position_), color(color_), normal(normal_), uv(uv_), id(id_) {}

  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 normal;
  glm::vec2 uv;
  float id;
};

class Primitives {
 public:
  template <class dtype>
  using vec4_t = std::array<dtype, 4>;
  using vec4f_t = vec4_t<float>;

  template <class dtype>
  using vec3_t = std::array<dtype, 3>;
  using vec3f_t = vec3_t<float>;

  enum class RenderType { NORMAL,
                          COLOR,
                          TEXTURE,
                          VERT_NORMAL };

  static RenderType getRenderType(std::string string) {
    RenderType renderType = RenderType::NORMAL;
    if (string == "NORMAL") {
      renderType = RenderType::NORMAL;
    } else if (string == "COLOR") {
      renderType = RenderType::COLOR;
    } else if (string == "TEXTURE") {
      renderType = RenderType::TEXTURE;
    }
    return renderType;
  }

 private:
  // nothing
 protected:
  std::string _name;
  GLuint _shaderID;
  Primitives::RenderType _defaultRenderType = Primitives::RenderType::COLOR;
  Primitives::RenderType _renderType = Primitives::RenderType::COLOR;
  bool _maskMode = false;
  bool _isVisible = true;
  glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 _vecocity = glm::vec3(0.0f, 0.0f, 0.0f);

 public:
  // nothing

 private:
  // nothing
 protected:
  // nothing

 public:
  void setMaskMode(bool maskMode) { _maskMode = maskMode; };
  void setVisible(bool isVisible) { _isVisible = isVisible; };
  void setName(std::string name) { _name = name; };
  void setShader(GLuint shaderID) { _shaderID = shaderID; };
  void setDefaultRenderType(Primitives::RenderType renderType) {
    _defaultRenderType = renderType;
    _renderType = renderType;
  };
  void resetRenderType() { _renderType = _defaultRenderType; };
  void setRenderType(Primitives::RenderType renderType) { _renderType = renderType; };
  std::string getName() { return _name; };
  float getRenderType() {
    return getRenderType(_maskMode, _renderType);
  };
  glm::vec3 getPosition() { return _position; };
  void setPosition(glm::vec3 position) { _position = position; };
  glm::vec3 getVecocity() { return _vecocity; };
  void setVecocity(glm::vec3 vecocity) { _vecocity = vecocity; };
  void forward(float deltaT) { _position = _position + deltaT * _vecocity; };
  bool* getPointerToIsVisible() { return &_isVisible; };

  virtual void update() { std::cout << "Primitives::update" << std::endl; };
  virtual void initVAO() { std::cout << "Primitives::initVAO" << std::endl; };
  virtual void paintGL(const glm::mat4& mvpMat){};
  virtual std::string getObjectType() = 0;

  inline static float getRenderType(bool maskMode, enum Primitives::RenderType renderType) {
    float renderTypeValue = 0.0f;

    if (maskMode) {
      renderTypeValue = -2.0f;
    } else if (renderType == Primitives::RenderType::NORMAL) {
      renderTypeValue = -1.0f;
    } else if (renderType == Primitives::RenderType::COLOR) {
      renderTypeValue = 0.0f;
    } else if (renderType == Primitives::RenderType::TEXTURE) {
      renderTypeValue = 1.0f;
    } else if (renderType == Primitives::RenderType::VERT_NORMAL) {
      renderTypeValue = -3.0f;
    }

    return renderTypeValue;
  };
};
#endif
