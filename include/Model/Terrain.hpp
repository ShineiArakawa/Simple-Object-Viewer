#pragma once
#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

class Terrain : public Primitives {
 private:
  std::string _filePath;
  int _indexBufferSize;
  float _offsetX;
  float _offsetY;
  float _offsetZ;
  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;

  // clang-format off
  inline static const glm::vec3 positions[4] = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 1.0f),
      glm::vec3(1.0f, 0.0f, 1.0f)
  };
  inline static const glm::vec3 colors[4] = {
      glm::vec3(4.0f / 255.0f, 200.0f / 255.0f, 3.0f / 255.0f), 
      glm::vec3(4.0f / 255.0f, 200.0f / 255.0f, 3.0f / 255.0f), 
      glm::vec3(4.0f / 255.0f, 200.0f / 255.0f, 3.0f / 255.0f), 
      glm::vec3(4.0f / 255.0f, 200.0f / 255.0f, 3.0f / 255.0f) 
  };
  inline static const glm::vec3 textureCoords[4] = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f)
  };
  // clang-format on

 protected:
  // nothing
 public:
  inline static const float TERRAIN_HEIGHT = 10.0f;
  inline static const float TERRAIN_WIDTH = 10.0f;
  inline static const float TERRAIN_HEIGHT_SCALE = 2.0f;

 private:
  // nothing
 protected:
  // nothing
 public:
  Terrain(const std::string& filePath, const float offsetX, const float offsetY, const float offsetZ);
  ~Terrain();

  void update(){};
  void initVAO();
  void paintGL(const glm::mat4& mvpMat);
};
#endif
