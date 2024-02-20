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
                          VERT_NORMAL,
                          SHADE,
                          SHADE_TEXTURE };

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
  void rotateObject(std::shared_ptr<std::vector<Vertex>> vertices, const float angle, const glm::vec3 axis) const {
    const int nVertices = vertices->size();
    const glm::mat4 rotMat = glm::rotate(angle, axis);

    for (int i_vertex = 0; i_vertex < nVertices; ++i_vertex) {
      const glm::vec4& position = rotMat * glm::vec4((*vertices)[i_vertex].position, 1.0f);
      const glm::vec4& normal = rotMat * glm::vec4((*vertices)[i_vertex].normal, 1.0f);
      (*vertices)[i_vertex].position = glm::vec3(position.x, position.y, position.z);
      (*vertices)[i_vertex].normal = glm::vec3(normal.x, normal.y, normal.z);
    }
  };

  void mergeVertices(
      const std::shared_ptr<std::vector<Vertex>>& sourceVertices,
      const std::shared_ptr<std::vector<unsigned int>>& sourceIndices,
      std::shared_ptr<std::vector<Vertex>>& distVertices,
      std::shared_ptr<std::vector<unsigned int>>& distIndices) {
    const int offsetIndex = distIndices->size();
    const int nSourceVertices = sourceVertices->size();

    for (int i_vertex = 0; i_vertex < nSourceVertices; ++i_vertex) {
      distVertices->push_back((*sourceVertices)[i_vertex]);
      distIndices->push_back((*sourceIndices)[i_vertex] + offsetIndex);
    }
  };

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
  glm::vec3 getPosition() { return _position; };
  void setPosition(glm::vec3 position) { _position = position; };
  glm::vec3 getVecocity() { return _vecocity; };
  void setVecocity(glm::vec3 vecocity) { _vecocity = vecocity; };
  void forward(float deltaT) { _position = _position + deltaT * _vecocity; };
  bool* getPointerToIsVisible() { return &_isVisible; };

  virtual void update() = 0;
  virtual void initVAO() = 0;
  virtual void paintGL(
      const glm::mat4& mvMat,
      const glm::mat4& mvpMat,
      const glm::mat4& normMat,
      const glm::mat4& lightMat,
      const glm::vec3& lightPos,
      const float& shininess,
      const float& ambientIntensity) = 0;
  virtual std::string getObjectType() = 0;

  float getRenderType() {
    return getRenderType(_maskMode, _renderType);
  };
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
    } else if (renderType == Primitives::RenderType::SHADE) {
      renderTypeValue = 2.0f;
    } else if (renderType == Primitives::RenderType::SHADE_TEXTURE) {
      renderTypeValue = 3.0f;
    }

    return renderTypeValue;
  };

  inline void bindShader(
      const glm::mat4& mvMat,
      const glm::mat4& mvpMat,
      const glm::mat4& normMat,
      const glm::mat4& lightMat,
      const glm::vec3& lightPos,
      const float& shininess,
      const float& ambientIntensity,
      const float& renderType,
      const bool& disableDepthTest = false) {
    GLuint uid;

    // Enable shader program
    glUseProgram(_shaderID);

    if (disableDepthTest) {
      glDisable(GL_DEPTH_TEST);
    }

    // Transfer uniform variables
    uid = glGetUniformLocation(_shaderID, "u_mvMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvMat));
    uid = glGetUniformLocation(_shaderID, "u_mvpMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(mvpMat));
    uid = glGetUniformLocation(_shaderID, "u_normMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(normMat));
    uid = glGetUniformLocation(_shaderID, "u_lightMat");
    glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(lightMat));
    uid = glGetUniformLocation(_shaderID, "u_lightPos");
    glUniform3fv(uid, 1, glm::value_ptr(lightPos));
    uid = glGetUniformLocation(_shaderID, "u_shininess");
    glUniform1f(uid, shininess);
    uid = glGetUniformLocation(_shaderID, "u_ambientIntensity");
    glUniform1f(uid, ambientIntensity);

    uid = glGetUniformLocation(_shaderID, "u_toUseTexture");
    glUniform1f(uid, renderType);
  };

  inline void unbindShader() {
    glEnable(GL_DEPTH_TEST);

    // Disable shader program
    glUseProgram(0);
  };
};
#endif
