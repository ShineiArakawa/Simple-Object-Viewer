#pragma once

#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <Util/FontStorage.hpp>
#include <Util/Texture.hpp>
#include <string>

namespace simview {
namespace model {

class TextBox : public Primitive {
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

  const char* _text;
  glm::vec4 _postionInScreenSpace = glm::vec4(0.0f);
  int _padding;
  std::shared_ptr<util::fonts::TTFFontRegistry> _fontRegistry;
  float _sizeMagnification;

 public:
  inline static const std::string KEY_MODEL_TEXT_BOX = "Text Box";

  explicit TextBox(
      const char* text,
      const glm::vec2 position,
      const float sizeMagnification = 1.0f,
      const int fontPixelsize = 64,
      const int padding = 8);

  ~TextBox();

  void update() override{};

  void initVAO() override;

  void paintGL(
      const TransformationContext& transCtx,  // transCtx
      const LightingContext& lightingCtx,     // lightingCtx
      const RenderingContext& renderingCtx    // renderingCtx
      ) override;

  void drawGL(const int& index = 0) override;

  void drawAllGL(const glm::mat4& lightMvpMat) override;

  std::string getObjectType() override { return KEY_MODEL_TEXT_BOX; };
};

using TextBox_t = std::shared_ptr<TextBox>;

}  // namespace model
}  // namespace simview