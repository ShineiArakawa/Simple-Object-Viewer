#pragma once

#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <Util/Texture.hpp>
#include <string>

namespace simview {
namespace model {

class Background : public Primitive {
 private:
  GLuint _vaoId;
  GLuint _vertexBufferId;
  GLuint _indexBufferId;
  GLuint _textureId;

  inline static const glm::vec3 positions[4] = {
      glm::vec3(1.0f, -1.0f, 1.0f),
      glm::vec3(1.0f, 1.0f, 1.0f),
      glm::vec3(-1.0f, -1.0f, 1.0f),
      glm::vec3(-1.0f, 1.0f, 1.0f)};

  inline static const glm::vec2 uvCoords[4] = {
      glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 0.0f),
      glm::vec2(0.0f, 1.0f),
      glm::vec2(0.0f, 0.0f)};

  std::string _textureFilePath;
  bool _isLoadedTexture;

 public:
  inline static const std::string KEY_MODEL_BACKGROUND = "Background";
  inline static const std::string KEY_MODEL_BACKGROUND_PATH = "Path";

  explicit Background(const GLuint&);
  explicit Background(const std::string& filePath);
  ~Background();
  void update() override{};
  void initVAO() override;
  void paintGL(
      const TransformationContext& transCtx,  // transCtx
      const LightingContext& lightingCtx,     // lightingCtx
      const RenderingContext& renderingCtx    // renderingCtx
      ) override;
  void drawGL(const int& index = 0) override;
  void drawAllGL(const glm::mat4& lightMvpMat) override;

  std::string getObjectType() override { return KEY_MODEL_BACKGROUND; };
};

using Background_t = std::shared_ptr<Background>;

}  // namespace model
}  // namespace simview