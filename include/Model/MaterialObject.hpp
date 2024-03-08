#pragma once

#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <Util/ObjectLoader.hpp>
#include <Util/Texture.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace simview {
namespace model {

class MaterialObject : public Primitives {
 public:
  struct MaterialObjectBuffer {
    MaterialObjectBuffer()
        : indexBufferSize(0),
          enabledAmbientTexture(false),
          enabledDiffuseTexture(false),
          enabledSpecularTexture(false),
          enabledBumpTexture(false),
          materialGroup(nullptr){};

    int indexBufferSize;
    GLuint vaoId;
    GLuint vertexBufferId;
    GLuint indexBufferId;

    GLuint ambientTextureId;
    GLuint diffuseTextureId;
    GLuint specularTextureId;
    GLuint bumpTextureId;

    bool enabledAmbientTexture;
    bool enabledDiffuseTexture;
    bool enabledSpecularTexture;
    bool enabledBumpTexture;

    MaterialGroup_t materialGroup;
  };

  using MaterialObjectBuffer_t = std::shared_ptr<MaterialObjectBuffer>;
  using MaterialObjectBuffers_t = std::shared_ptr<std::vector<MaterialObjectBuffer_t>>;

 private:
  std::string _filePath;
  glm::vec3 _offset;
  glm::vec3 _scale;

  MaterialObjectBuffers_t _materialObjectBuffers = nullptr;

 protected:
  // nothing
 public:
  inline static const std::string KEY_MODEL_MATERIAL_OBJECT = "Object with material";
  inline static const std::string KEY_MODEL_MATERIAL_OBJECT_NAME = "Name";

 private:
  // nothing
 protected:
  // nothing
 public:
  MaterialObject(const std::string& filePath, const glm::vec3 offset, const glm::vec3 scale);
  ~MaterialObject();
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

  std::string getObjectType() override { return KEY_MODEL_MATERIAL_OBJECT; };
};

using MaterialObject_t = std::shared_ptr<MaterialObject>;

}  // namespace model
}  // namespace simview