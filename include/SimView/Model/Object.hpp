#pragma once

#include <SimView/Model/Primitives.hpp>
#include <SimView/OpenGL.hpp>
#include <SimView/Util/ObjectLoader.hpp>
#include <SimView/Util/Texture.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace simview {
namespace model {

class Object : public Primitive {
 private:
  std::string _filePath;
  float _offsetX;
  float _offsetY;
  float _offsetZ;
  float _scale;
  bool _autoScale;
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
  Object(const std::string& filePath = "",
         const float offsetX = 0.0f,
         const float offsetY = 0.0f,
         const float offsetZ = 0.0f,
         const float scale = 1.0f,
         const bool autoScale = false);
  ~Object();
  void loadTexture(const std::string& filePath);
  void loadNormalMap(const std::string& filePath);
  void update() override{};
  void initVAO() override;
  void initVAO(const VertexArray_t&,
               const IndexArray_t&);
  void initVAO(const std::shared_ptr<std::vector<vec3f_t>> positions,
               const std::shared_ptr<std::vector<vec3f_t>> colors = nullptr,
               const std::shared_ptr<std::vector<vec3f_t>> normals = nullptr,
               const std::shared_ptr<std::vector<vec2f_t>> uvCoords = nullptr,
               const std::shared_ptr<std::vector<int>> ids = nullptr);
  void paintGL(const TransformationContext& transCtx,  // transCtx
      const LightingContext& lightingCtx,     // lightingCtx
      const RenderingContext& renderingCtx    // renderingCtx
      ) override;
  void drawGL(const int& index = 0) override;
  void drawAllGL(const glm::mat4& lightMvpMat) override;

  std::string getObjectType() override { return KEY_MODEL_OBJECT; };
};

using Object_t = std::shared_ptr<Object>;

}  // namespace model
}  // namespace simview