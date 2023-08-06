#pragma once
#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <Util/ObjectLoader.hpp>
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
  float _scaleX;
  float _scaleY;
  float _scaleH;
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
  // nothing
 private:
  // nothing
 protected:
  // nothing
 public:
  Terrain(const std::string& filePath, const float offsetX, const float offsetY, const float offsetZ, const float scaleX, const float scaleY,
          const float scaleH);
  ~Terrain();

  void update(){};
  void initVAO();
  void paintGL(const glm::mat4& mvpMat);
};
#endif
