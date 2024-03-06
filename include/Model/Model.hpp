#pragma once

#include <Model/Background.hpp>
#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <Shader/DefaultShaders.hpp>
#include <Shader/ModelShader.hpp>
#include <Shader/Shader.hpp>
#include <Shader/ShaderCompiler.hpp>
#include <Util/Logging.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace simview {
namespace model {
class Model {
 public:
  using t_object = std::shared_ptr<Primitives>;
  using t_background = std::shared_ptr<Background>;
  using t_objects = std::shared_ptr<std::vector<t_object>>;
  using t_string = std::shared_ptr<std::string>;

 private:
  // Nothing

 protected:
  t_objects _objects = std::make_shared<std::vector<t_object>>();
  t_objects _backgrounds = std::make_shared<std::vector<t_object>>();
  glm::vec4 _backgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

  t_string _modelVertMShaderPath = nullptr;
  t_string _modelFragShaderPath = nullptr;
  t_string _depthVertMShaderPath = nullptr;
  t_string _depthFragShaderPath = nullptr;

  int _backgroundIDtoDraw = 0;
  float _time = 0.0f;

  Primitives::ModelShader_t _shader = nullptr;
  Primitives::DepthShader_t _depthShader = nullptr;

  glm::vec4 _lightPosition = glm::vec4(5.0f, 0.0f, 0.0f, 1.0f);
  float _shininess = 50.0f;
  float _ambientIntensity = 0.1f;
  glm::vec3 _wireFrameColor = glm::vec3(1.0f, 1.0f, 1.0f);
  float _wireFrameWidth = 0.01f;

 public:
  // clang-format off
  inline static const std::string KEY_MODEL = "Model";
  inline static const std::string KEY_BACKGROUND = "Background";
  inline static const std::string KEY_BACKGROUND_COLOR = "Color";
  // clang-format on

 public:
  virtual ~Model();
  virtual void initVAO() = 0;
  virtual void paintGL(const glm::mat4 &mvMat,
                       const glm::mat4 &mvpMat,
                       const glm::mat4 &lightMat,
                       const glm::mat4 &lightMvpMat,
                       const GLuint &depthMapId) = 0;
  virtual void tick(float time) = 0;

  void drawGL(const glm::mat4 &lightMvpMat) {
    for (const auto &object : *_objects) {
      object->drawAllGL(lightMvpMat);
    }
  }

  void compileShaders(const bool &isQuad = false);

  void addBackground(const t_background &background) {
    background->setModelShader(getModelShader());
    background->setDepthShader(getDepthShader());
    background->initVAO();
    _backgrounds->push_back(background);
  };

  void addObject(const t_object &object, bool toInitializeVAO = true) {
    object->setModelShader(getModelShader());
    object->setDepthShader(getDepthShader());
    if (toInitializeVAO) {
      object->initVAO();
    }
    _objects->push_back(object);
  };

  void removeObject(const int index) {
    if (index >= 0 && index < getNumObjects()) {
      _objects->erase(_objects->begin() + index);
    }
  }

  void removeBackground(const int index) {
    if (index >= 0 && index < getNumBackgrounds()) {
      _backgrounds->erase(_backgrounds->begin() + index);
    }
  }

  t_objects getBackgrounds() { return _backgrounds; };

  Primitives::ModelShader_t getModelShader() { return _shader; };

  Primitives::DepthShader_t getDepthShader() { return _depthShader; };

  t_object getBackground(const int index) { return (*_backgrounds)[index]; };

  int getNumBackgrounds() { return (int)_backgrounds->size(); };

  t_objects getObjects() { return _objects; };

  t_object getObject(const int index) { return (*_objects)[index]; };

  int getNumObjects() { return (int)_objects->size(); };

  int getBackgroundIDtoDraw() { return _backgroundIDtoDraw; };

  glm::vec4 getBackgroundColor() { return _backgroundColor; };

  glm::vec4 getLightPos() { return _lightPosition; };

  float getShininess() { return _shininess; };

  float getAmbientIntensity() { return _ambientIntensity; };

  glm::vec3 getWireFrameColor() { return _wireFrameColor; };

  float getWireFrameWidth() { return _wireFrameWidth; };

  void setModelVertShaderPath(const t_string &vertShaderPath) { _modelVertMShaderPath = vertShaderPath; };

  void setModelFragShaderPath(const t_string &fragShaderPath) { _modelFragShaderPath = fragShaderPath; };

  void setDepthVertShaderPath(const t_string &vertShaderPath) { _depthVertMShaderPath = vertShaderPath; };

  void setDepthFragShaderPath(const t_string &fragShaderPath) { _depthFragShaderPath = fragShaderPath; };

  void setModelShader(Primitives::ModelShader_t shader);

  void setDepthShader(Primitives::DepthShader_t shader);

  void setMaskMode(bool maskMode);

  void setRenderType(Primitives::RenderType renderType);

  void setIsEnabledNormalMap(bool isEnabledNormalMap);

  void setIsEnabledShadowMapping(bool isEnabledShadowMapping);

  void setIsVisibleBBOX(bool isEnabledShadowMapping);

  void setWireFrameMode(Primitives::WireFrameMode wireFrameMode);

  void setBackgroundIDtoDraw(const int index) { _backgroundIDtoDraw = index; };

  void resetRenderType();

  void setBackgroundColor(const float &r, const float &g, const float &b, const float &a) {
    _backgroundColor.x = r;
    _backgroundColor.y = g;
    _backgroundColor.z = b;
    _backgroundColor.w = a;
  }

  void setWireFrameColor(const float &r, const float &g, const float &b) {
    _wireFrameColor.x = r;
    _wireFrameColor.y = g;
    _wireFrameColor.z = b;
  }

  void setLightPosition(const float &x, const float &y, const float &z, const float &w = 1.0f) {
    _lightPosition.x = x;
    _lightPosition.y = y;
    _lightPosition.z = z;
    _lightPosition.w = w;
  }

  void setBackgroundColor(const glm::vec4 &rgba) { _backgroundColor = rgba; }

  void setLightPosition(const glm::vec4 &lightPos) { _lightPosition = lightPos; }

  void setShiniess(const float &shininess) { _shininess = shininess; }

  void setAmbientIntensity(const float &ambientIntensity) { _ambientIntensity = ambientIntensity; }

  void setWireFrameColor(const glm::vec3 &wireFrameColor) { _wireFrameColor = wireFrameColor; }

  void setWireFrameWidth(const float &wireFrameWidth) { _wireFrameWidth = wireFrameWidth; }
};
}  // namespace model
}  // namespace simview