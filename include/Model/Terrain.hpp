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
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(1.0f, 0.0f, 1.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)
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
    inline static const unsigned int faces[2][3] = {
            {0, 1, 2},
            {2, 3, 0}};
  // clang-format on

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_TERRAIN = "Terrain";
  inline static const std::string KEY_MODEL_TERRAIN_HEIGHT_MAP_PATH = "HeightMapPath";
  inline static const std::string KEY_MODEL_TERRAIN_CENTER = "Center";
  inline static const std::string KEY_MODEL_TERRAIN_SCALE_XY = "ScaleXY";
  inline static const std::string KEY_MODEL_TERRAIN_SCALE_H = "ScaleH";

 private:
  // nothing
 protected:
  // nothing
 public:
  Terrain(const std::string& filePath, float offsetX, float offsetY, float offsetZ, float scaleX, float scaleY, float scaleH);
  ~Terrain();

  void update() override{};
  void initVAO() override;
  void paintGL(const glm::mat4& mvMat,
               const glm::mat4& mvpMat,
               const glm::mat4& normMat,
               const glm::mat4& lightMat,
               const glm::vec3& lightPos,
               const float& shininess,
               const float& ambientIntensity) override;

  std::string getObjectType() override { return KEY_MODEL_TERRAIN; };
};

#endif
