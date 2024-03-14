#pragma once

#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <Util/ObjectLoader.hpp>
#include <Util/Texture.hpp>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace simview {
namespace model {

class Box : public Primitive {
 private:
  std::string _filePath;
  float _offsetX;
  float _offsetY;
  float _offsetZ;
  float _scaleX;
  float _scaleY;
  float _scaleZ;
  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;
  GLuint _textureId;

  inline static const glm::vec3 positions[8] = {
      glm::vec3(-1.0f, -1.0f, -1.0f),
      glm::vec3(1.0f, -1.0f, -1.0f),
      glm::vec3(-1.0f, 1.0f, -1.0f),
      glm::vec3(-1.0f, -1.0f, 1.0f),
      glm::vec3(1.0f, 1.0f, -1.0f),
      glm::vec3(-1.0f, 1.0f, 1.0f),
      glm::vec3(1.0f, -1.0f, 1.0f),
      glm::vec3(1.0f, 1.0f, 1.0f)};
  inline static const glm::vec3 colors[6] = {
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 1.0f),
      glm::vec3(1.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 1.0f),
      glm::vec3(1.0f, 0.0f, 1.0f)};
  inline static const glm::vec3 textureCoords[8] = {
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f)};
  inline static const unsigned int faces[12][3] = {
      {7, 4, 1},
      {7, 1, 6},
      {2, 4, 7},
      {2, 7, 5},
      {5, 7, 6},
      {5, 6, 3},
      {4, 2, 0},
      {4, 0, 1},
      {3, 6, 1},
      {3, 1, 0},
      {2, 5, 3},
      {2, 3, 0}};
  inline static const glm::vec2 uvKeypointCoords[14] = {
      glm::vec2(0.0, 2.0 / 3.0),        // 0
      glm::vec2(1.0 / 4.0, 2.0 / 3.0),  // 1
      glm::vec2(1.0 / 4.0, 1.0),        // 2
      glm::vec2(2.0 / 4.0, 1.0),        // 3
      glm::vec2(2.0 / 4.0, 2.0 / 3.0),  // 4
      glm::vec2(3.0 / 4.0, 2.0 / 3.0),  // 5
      glm::vec2(1.0, 2.0 / 3.0),        // 6
      glm::vec2(1.0, 1.0 / 3.0),        // 7
      glm::vec2(3.0 / 4.0, 1.0 / 3.0),  // 8
      glm::vec2(2.0 / 4.0, 1.0 / 3.0),  // 9
      glm::vec2(2.0 / 4.0, 0.0),        // 10
      glm::vec2(1.0 / 4.0, 0.0),        // 11
      glm::vec2(1.0 / 4.0, 1.0 / 3.0),  // 12
      glm::vec2(0.0, 1.0 / 3.0),        // 13
  };
  inline static const unsigned int faceToUVKeypointIndex[12][3] = {
      {4, 9, 12}, {4, 12, 1},  // =3
      {8, 9, 4},
      {8, 4, 5},  // =5
      {3, 4, 1},
      {3, 1, 2},  // =1
      {9, 10, 11},
      {9, 11, 12},  // =6
      {0, 1, 12},
      {0, 12, 13},  // =2
      {8, 5, 6},
      {8, 6, 7}  // =4
  };
  inline static const glm::vec3 normals[6] = {
      glm::vec3(1.0f, 0.0f, 0.0f),   // 0
      glm::vec3(0.0f, 1.0f, 0.0f),   // 1
      glm::vec3(0.0f, 0.0f, 1.0f),   // 2
      glm::vec3(0.0f, 0.0f, -1.0f),  // 3
      glm::vec3(0.0f, -1.0f, 0.0f),  // 4
      glm::vec3(-1.0f, 0.0f, 0.0f)   // 5
  };

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_BOX = "Box";
  inline static const std::string KEY_MODEL_BOX_CENTER = "Center";
  inline static const std::string KEY_MODEL_BOX_TEXTURE_PATH = "TexturePath";
  inline static const std::string KEY_MODEL_BOX_SCALE = "Scale";

 private:
  // nothing
 protected:
  // nothing
 public:
  Box(float offsetX, float offsetY, float offsetZ, float scaleX, float scaleY, float scaleZ);
  ~Box();
  void loadTexture(const std::string& filePath);
  void update() override{};
  void initVAO() override;
  void paintGL(
      const TransformationContext& transCtx,  // transCtx
      const LightingContext& lightingCtx,     // lightingCtx
      const RenderingContext& renderingCtx    // renderingCtx
      ) override;
  void drawGL(const int& index = 0) override;
  void drawAllGL(const glm::mat4& lightMvpMat) override;

  std::string getObjectType() override { return KEY_MODEL_BOX; };
};

using Box_t = std::shared_ptr<Box>;

}  // namespace model
}  // namespace simview