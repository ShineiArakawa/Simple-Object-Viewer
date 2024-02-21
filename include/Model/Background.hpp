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
  inline static const std::string KEY_MODEL_BACKGROUND = "Background";
  inline static const std::string KEY_MODEL_BACKGROUND_PATH = "Path";

  explicit Background(const std::string& filePath);
  ~Background();
  void update() override{};
  void initVAO() override;
  void paintGL(const glm::mat4& mvMat,
               const glm::mat4& mvpMat,
               const glm::mat4& normMat,
               const glm::mat4& lightMat,
               const glm::vec3& lightPos,
               const float& shininess,
               const float& ambientIntensity,
               const glm::vec3& wireFrameColor,
               const float& wireFrameWidth) override;

  std::string getObjectType() override { return KEY_MODEL_BACKGROUND; };
};

#endif