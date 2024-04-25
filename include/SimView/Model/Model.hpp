#pragma once

#include <SimView/Model/Background.hpp>
#include <SimView/Model/Primitives.hpp>
#include <SimView/Model/RenderingContext.hpp>
#include <SimView/OpenGL.hpp>
#include <SimView/Shader/DefaultShaders.hpp>
#include <SimView/Shader/ModelShader.hpp>
#include <SimView/Shader/Shader.hpp>
#include <SimView/Shader/ShaderCompiler.hpp>
#include <SimView/Util/Logging.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace simview {
namespace model {

class Model {
 public:
  using Objects_t = std::shared_ptr<std::vector<Primitive_t>>;
  using Backgrounds_t = std::shared_ptr<std::vector<Background_t>>;
  using String_t = std::shared_ptr<std::string>;

 private:
  // Nothing

 protected:
  Objects_t _objects = nullptr;
  Backgrounds_t _backgrounds = nullptr;
  glm::vec4 _backgroundColor;

  String_t _modelVertMShaderPath = nullptr;
  String_t _modelFragShaderPath = nullptr;
  String_t _depthVertMShaderPath = nullptr;
  String_t _depthFragShaderPath = nullptr;

  int _backgroundIDtoDraw;
  float _time;

  shader::ModelShader_t _shader = nullptr;
  shader::DepthShader_t _depthShader = nullptr;

  glm::vec4 _lightPosition;
  float _shininess;
  float _ambientIntensity;
  glm::vec3 _wireFrameColor;
  float _wireFrameWidth;

 public:
  // clang-format off
  inline static const std::string KEY_MODEL                      = "Model";
  inline static const std::string KEY_BACKGROUND                 = "Background";
  inline static const std::string KEY_BACKGROUND_COLOR           = "Color";
  // clang-format on

 public:
  Model();
  virtual ~Model();
  virtual void initVAO() = 0;
  virtual void paintGL(const TransformationContext &transCtx,  // transCtx
                       const GLuint &depthMapId                // depthMapId
                       ) = 0;
  virtual void tick(float time) = 0;

  void drawGL(const glm::mat4 &lightMvpMat) {
    for (const auto &object : *_objects) {
      object->drawAllGL(lightMvpMat);
    }
  }

  void compileShaders(const bool &isQuad = false);

  void addBackground(const Background_t &background) {
    try {
      background->setModelShader(getModelShader());
      background->setDepthShader(getDepthShader());
      background->initVAO();
      _backgrounds->push_back(background);
    } catch (std::exception exception) {
      LOG_ERROR("Failed to add a new object. Please check attributes!");
      LOG_ERROR(exception.what());
    }
  };

  void addObject(const Primitive_t &object, bool toInitializeVAO = true) {
    try {
      object->setModelShader(getModelShader());
      object->setDepthShader(getDepthShader());
      if (toInitializeVAO) {
        object->initVAO();
      }
      _objects->push_back(object);
    } catch (std::exception exception) {
      LOG_ERROR("Failed to add a new object. Please check attributes!");
      LOG_ERROR(exception.what());
    }
  };

  void removeObject(const int index) {
    if (index >= 0 && index < getNumObjects()) {
      _objects->erase(_objects->begin() + index);
    }
  }

  void removeObject(const std::string name) {
    for (int iObject = 0; iObject < (int)_objects->size(); ++iObject) {
      if ((*_objects)[iObject]->getName() == name) {
        _objects->erase(_objects->begin() + iObject);
        return;  // NOTE: erase one objest at most
      }
    }
  }

  void removeBackground(const int index) {
    if (index >= 0 && index < getNumBackgrounds()) {
      _backgrounds->erase(_backgrounds->begin() + index);
    }
  }

  Backgrounds_t getBackgrounds() const { return _backgrounds; };

  shader::ModelShader_t getModelShader() const { return _shader; };

  shader::DepthShader_t getDepthShader() const { return _depthShader; };

  Background_t getBackground(const int index) const {
    return (*_backgrounds)[index];
  };

  int getNumBackgrounds() const { return (int)_backgrounds->size(); };

  Objects_t getObjects() const { return _objects; };

  Primitive_t getObject(const int index) const { return (*_objects)[index]; };

  std::vector<Primitive_t> getObjectByType(const std::string &type) const {
    std::vector<Primitive_t> objects;

    for (const auto &item : *_objects) {
      if (item->getObjectType() == type) {
        objects.push_back(item);
      }
    }

    return objects;
  }

  int getNumObjects() const { return (int)_objects->size(); };

  int getBackgroundIDtoDraw() const { return _backgroundIDtoDraw; };

  glm::vec4 getBackgroundColor() const { return _backgroundColor; };

  glm::vec4 getLightPos() const { return _lightPosition; };

  float getShininess() const { return _shininess; };

  float getAmbientIntensity() const { return _ambientIntensity; };

  glm::vec3 getWireFrameColor() const { return _wireFrameColor; };

  float getWireFrameWidth() const { return _wireFrameWidth; };

  void setModelVertShaderPath(const String_t &vertShaderPath) {
    _modelVertMShaderPath = vertShaderPath;
  };

  void setModelFragShaderPath(const String_t &fragShaderPath) {
    _modelFragShaderPath = fragShaderPath;
  };

  void setDepthVertShaderPath(const String_t &vertShaderPath) {
    _depthVertMShaderPath = vertShaderPath;
  };

  void setDepthFragShaderPath(const String_t &fragShaderPath) {
    _depthFragShaderPath = fragShaderPath;
  };

  void setModelShader(shader::ModelShader_t shader);

  void setDepthShader(shader::DepthShader_t shader);

  void setMaskMode(bool maskMode);

  void setRenderType(Primitive::RenderType renderType);

  void setIsEnabledNormalMap(bool isEnabledNormalMap);

  void setIsEnabledShadowMapping(bool isEnabledShadowMapping);

  void setIsVisibleBBOX(bool isEnabledShadowMapping);

  void setWireFrameMode(Primitive::WireFrameMode wireFrameMode);

  void setBackgroundIDtoDraw(const int index) { _backgroundIDtoDraw = index; };

  void resetRenderType();

  void setBackgroundColor(const float &r, const float &g, const float &b,
                          const float &a) {
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

  void setLightPosition(const float &x, const float &y, const float &z,
                        const float &w = 1.0f) {
    _lightPosition.x = x;
    _lightPosition.y = y;
    _lightPosition.z = z;
    _lightPosition.w = w;
  }

  void setBackgroundColor(const glm::vec4 &rgba) { _backgroundColor = rgba; }

  void setLightPosition(const glm::vec4 &lightPos) {
    _lightPosition = lightPos;
  }

  void setShiniess(const float &shininess) { _shininess = shininess; }

  void setAmbientIntensity(const float &ambientIntensity) {
    _ambientIntensity = ambientIntensity;
  }

  void setWireFrameColor(const glm::vec3 &wireFrameColor) {
    _wireFrameColor = wireFrameColor;
  }

  void setWireFrameWidth(const float &wireFrameWidth) {
    _wireFrameWidth = wireFrameWidth;
  }
};

using Model_t = std::shared_ptr<Model>;

}  // namespace model
}  // namespace simview