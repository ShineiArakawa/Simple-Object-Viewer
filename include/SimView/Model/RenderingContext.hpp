#pragma once

#include <SimView/OpenGL.hpp>

namespace simview {
namespace model {

struct TransformationContext {
  TransformationContext(
      const glm::mat4 &mvMat_,         // mvMat
      const glm::mat4 &mvpMat_,        // mvpMat
      const glm::mat4 &lightMat_,      // lightMat
      const glm::mat4 &lightMvpMat_,   // lightMvpMat
      const glm::mat4 &modelMat_,      // modelMat
      const glm::mat4 &viewMat_,       // viewMat
      const glm::mat4 &projMat_,       // projMat
      const glm::mat4 &transMat_,      // transMat
      const glm::mat4 &rotMat_,        // rotMat
      const glm::mat4 &scaleMat_,      // scaleMat
      const glm::mat4 &lightTransMat_  // lightTransMat
      ) : mvMat(mvMat_),
          mvpMat(mvpMat_),
          lightMat(lightMat_),
          lightMvpMat(lightMvpMat_),
          modelMat(modelMat_),
          viewMat(viewMat_),
          projMat(projMat_),
          transMat(transMat_),
          rotMat(rotMat_),
          scaleMat(scaleMat_),
          lightTransMat(lightTransMat_) {}

  const glm::mat4 &mvMat;
  const glm::mat4 &mvpMat;
  const glm::mat4 &lightMat;
  const glm::mat4 &lightMvpMat;
  const glm::mat4 &lightTransMat;

  // [Redundant]
  const glm::mat4 &modelMat;  // modelMat
  const glm::mat4 &viewMat;   // viewMat
  const glm::mat4 &projMat;   // projMat
  const glm::mat4 &transMat;  // transMat
  const glm::mat4 &rotMat;    // rotMat
  const glm::mat4 &scaleMat;  // scaleMat
};

struct LightingContext {
  LightingContext(
      const glm::vec3 &lightPos_,     // lightPos
      const float &shininess_,        // shininess
      const float &ambientIntensity_  // ambientIntensity
      ) : lightPos(lightPos_),
          shininess(shininess_),
          ambientIntensity(ambientIntensity_) {}

  const glm::vec3 &lightPos;
  const float &shininess;
  const float &ambientIntensity;
};

struct RenderingContext {
  RenderingContext(
      const glm::vec3 &wireFrameColor_,  // wireFrameColor
      const float &wireFrameWidth_,      // wireFrameWidth
      const GLuint &depthTextureId_      // depthTextureId
      ) : wireFrameColor(wireFrameColor_),
          wireFrameWidth(wireFrameWidth_),
          depthTextureId(depthTextureId_) {}

  const glm::vec3 &wireFrameColor;
  const float &wireFrameWidth;
  const GLuint &depthTextureId;
};

}  // namespace model
}  // namespace simview
