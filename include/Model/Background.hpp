#pragma once
#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <Util/Texture.hpp>
#include <string>

class Background : public Primitives {
 private:
  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;
  GLuint _textureId;

  // clang-format off
  inline static const glm::vec3 positions[4] = {
    glm::vec3(1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, -1.0f, 1.0f),
    glm::vec3(-1.0f, 1.0f, 1.0f)
  };

  inline static const glm::vec2 uvCoords[4] = {
    glm::vec2(1.0f, 1.0f),
    glm::vec2(1.0f, 0.0f),
    glm::vec2(0.0f, 1.0f),
    glm::vec2(0.0f, 0.0f)
  };
  // clang-format on

 public:
  Background(const std::string& filePath);
  ~Background();

  void update(){};
  void initVAO();
  void paintGL(const glm::mat4& mvpMat);
};

#endif