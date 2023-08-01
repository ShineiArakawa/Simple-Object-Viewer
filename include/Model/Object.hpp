#pragma once
#ifndef _MODEL_OBJECT_H_
#define _MODEL_OBJECT_H_

#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <Util/ObjectLoader.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Object : public Primitives {
 private:
  std::string _filePath;
  int _indexBufferSize;
  float _offsetX;
  float _offsetY;
  float _offsetZ;
  float _scale;
  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;
  GLuint _textureId;

 protected:
  // nothing
 public:
  // nothing

 private:
  // nothing
 protected:
  // nothing
 public:
  Object(const std::string& filePath, const float offsetX, const float offsetY, const float offsetZ, const float scale = 1.0f);
  ~Object();

  void loadTexture(const std::string& filePath);
  void update(){};
  void initVAO();
  void paintGL(const glm::mat4& mvpMat);
};
#endif
