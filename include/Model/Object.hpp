#pragma once
#ifndef _MODEL_OBJECT_H_
#define _MODEL_OBJECT_H_

#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <Util/ObjectLoader.hpp>
#include <Util/Texture.hpp>
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
  GLuint _normalMapId;

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_OBJECT = "Object";
  inline static const std::string KEY_MODEL_OBJECT_NAME = "Name";
  inline static const std::string KEY_MODEL_OBJECT_OBJ_PATH = "ObjPath";
  inline static const std::string KEY_MODEL_OBJECT_TEXTURE_PATH = "TexturePath";
  inline static const std::string KEY_MODEL_OBJECT_DEFAULT_RENDER_TYPE = "DefaultRenderType";
  inline static const std::string KEY_MODEL_OBJECT_SCALE = "Scale";
  inline static const std::string KEY_MODEL_OBJECT_OFFSET = "Offset";

 private:
  // nothing
 protected:
  // nothing
 public:
  Object(const std::string& filePath, float offsetX, float offsetY, float offsetZ, float scale = 1.0f);
  ~Object();
  void loadTexture(const std::string& filePath);
  void loadNormalMap(const std::string& filePath);
  void update() override{};
  void initVAO() override;
  void paintGL(const glm::mat4& mvMat,
               const glm::mat4& mvpMat,
               const glm::mat4& lightMat,
               const glm::vec3& lightPos,
               const float& shininess,
               const float& ambientIntensity,
               const glm::vec3& wireFrameColor,
               const float& wireFrameWidth,
               const GLuint& depthTextureId,
               const glm::mat4& lightMvpMat) override;
  void drawGL(const int& index = 0) override;
  void drawAllGL(const glm::mat4& lightMvpMat) override;

  std::string getObjectType() override { return KEY_MODEL_OBJECT; };
};

#endif
