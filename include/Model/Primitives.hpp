#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <Model/AxisAlignedBoundingBox.hpp>
#include <OpenGL.hpp>
#include <Shader/DefaultShaders.hpp>
#include <Shader/DepthShader.hpp>
#include <Shader/ModelShader.hpp>
#include <Util/DataStructure.hpp>
#include <Util/Logging.hpp>
#include <Util/Math.hpp>
#include <array>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace simview {
namespace model {

class Primitives {
  // ==================================================================================================
  // Type defines
  // ==================================================================================================
 public:
  enum class RenderType {
    NORMAL,
    COLOR,
    TEXTURE,
    VERT_NORMAL,
    SHADE,
    SHADE_TEXTURE,
    MATERIAL
  };

  static RenderType getRenderType(std::string string) {
    RenderType renderType = RenderType::NORMAL;
    if (string == "NORMAL") {
      renderType = RenderType::NORMAL;
    } else if (string == "COLOR") {
      renderType = RenderType::COLOR;
    } else if (string == "TEXTURE") {
      renderType = RenderType::TEXTURE;
    }
    return renderType;
  }

  enum class WireFrameMode {
    OFF,
    ON,
    ONLY
  };

  // ==================================================================================================
  // Variable defines
  // ==================================================================================================
 private:
  // nothing
 protected:
  std::string _name;
  shader::ModelShader_t _shader = nullptr;
  shader::DepthShader_t _depthShader = nullptr;
  RenderType _defaultRenderType = RenderType::COLOR;
  RenderType _renderType = RenderType::COLOR;
  WireFrameMode _wireFrameMode = WireFrameMode::OFF;
  bool _maskMode = false;
  bool _isVisible = true;
  bool _isEnabledNormalMap = false;
  bool _isEnabledShadowMapping = false;
  bool _isVisibleBBOX = true;
  glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 _vecocity = glm::vec3(0.0f, 0.0f, 0.0f);
  AxisAlignedBoundingBox_t _bbox = nullptr;

 public:
  // nothing

  // ==================================================================================================
  // Function defines
  // ==================================================================================================
 private:
  // nothing

 protected:
  // nothing

 public:
  void setMaskMode(bool maskMode) { _maskMode = maskMode; };
  void setVisible(bool isVisible) { _isVisible = isVisible; };
  void setIsEnabledNormalMap(bool isEnabledNormalMap) { _isEnabledNormalMap = isEnabledNormalMap; };
  void setIsEnabledShadowMapping(bool isEnabledShadowMapping) { _isEnabledShadowMapping = isEnabledShadowMapping; };
  void setVisibleBBOX(bool isVisibleBBOX) { _isVisibleBBOX = isVisibleBBOX; };
  void setName(std::string name) { _name = name; };
  void setModelShader(shader::ModelShader_t shader) { _shader = shader; };
  void setDepthShader(shader::DepthShader_t shader) { _depthShader = shader; };
  std::string getName() { return _name; };
  glm::vec3 getPosition() { return _position; };
  void setPosition(glm::vec3 position) { _position = position; };
  glm::vec3 getVecocity() { return _vecocity; };
  void setVecocity(glm::vec3 vecocity) { _vecocity = vecocity; };
  void forward(float deltaT) { _position = _position + deltaT * _vecocity; };
  bool* getPointerToIsVisible() { return &_isVisible; };

  virtual std::string getObjectType() = 0;

  // ==================================================================================================
  // Rendering options
  // ==================================================================================================
  void setDefaultRenderType(const RenderType& renderType) {
    _defaultRenderType = renderType;
    _renderType = renderType;
  };

  void resetRenderType() { _renderType = _defaultRenderType; };

  void setRenderType(const RenderType& renderType) { _renderType = renderType; };

  float getRenderType() {
    return getRenderType(_maskMode, _renderType);
  };

  void setWireFrameMode(const WireFrameMode& wireFrameMode) { _wireFrameMode = wireFrameMode; };

  float getWireFrameMode() const {
    return getWireFrameMode(_wireFrameMode);
  };

  inline static float getRenderType(const bool& maskMode, const RenderType& renderType) {
    float renderTypeValue = 0.0f;

    if (maskMode) {
      renderTypeValue = -2.0f;
    } else if (renderType == RenderType::NORMAL) {
      renderTypeValue = -1.0f;
    } else if (renderType == RenderType::COLOR) {
      renderTypeValue = 0.0f;
    } else if (renderType == RenderType::TEXTURE) {
      renderTypeValue = 1.0f;
    } else if (renderType == RenderType::VERT_NORMAL) {
      renderTypeValue = -3.0f;
    } else if (renderType == RenderType::SHADE) {
      renderTypeValue = 2.0f;
    } else if (renderType == RenderType::SHADE_TEXTURE) {
      renderTypeValue = 3.0f;
    } else if (renderType == RenderType::MATERIAL) {
      renderTypeValue = 4.0f;
    }

    return renderTypeValue;
  };

  inline static float getWireFrameMode(const WireFrameMode& wireFrameType) {
    float wireFrameTypeValue = 0.0f;

    if (wireFrameType == WireFrameMode::ON) {
      wireFrameTypeValue = 1.0f;
    } else if (wireFrameType == WireFrameMode::ONLY) {
      wireFrameTypeValue = -1.0f;
    }

    return wireFrameTypeValue;
  };

  // ==================================================================================================
  // Rendering
  // ==================================================================================================
  inline void bindShader(
      const glm::mat4& mvMat,
      const glm::mat4& mvpMat,
      const glm::mat4& normMat,
      const glm::mat4& lightMat,
      const glm::vec3& lightPos,
      const float& shininess,
      const float& ambientIntensity,
      const glm::vec3& ambientColor,
      const glm::vec3& diffuseColor,
      const glm::vec3& specularColor,
      const float& renderType,
      const float& wireFrameMode,
      const glm::vec3& wireFrameColor,
      const float& wireFrameWidth,
      const GLuint& depthTextureId,
      const glm::mat4& lightMvpMat,
      const bool& disableShadowMapping = false,
      const bool& disableDepthTest = false,
      const bool& isEnabledNormalMap = false) const {
    _shader->bind(disableDepthTest);

    // Transfer uniform variables
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_MV_MAT, mvMat);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_MVP_MAT, mvpMat);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_NORM_MAT, normMat);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_LIGHT_MAT, lightMat);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_LIGHT_POS, lightPos);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_SHININESS, shininess);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_AMBIENT_INTENSITY, ambientIntensity);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_AMBIENT_COLOR, ambientColor);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_DIFFUSE_COLOR, diffuseColor);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_SPECULAR_COLOR, specularColor);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_WIRE_FRAME_MODE, wireFrameMode);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_WIRE_FRAME_COLOR, wireFrameColor);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_WIRE_FRAME_WIDTH, wireFrameWidth);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_RENDER_TYPE, renderType);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_BUMP_MAP, isEnabledNormalMap);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_LIGHT_MVP_MAT, lightMvpMat);
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_SHADOW_MAPPING, _isEnabledShadowMapping);

    _shader->setUniformTexture(shader::DefaultModelShader::UNIFORM_NAME_DEPTH_TEXTURE, depthTextureId);
  };

  inline void unbindShader() const {
    _shader->unbind();
  };

  inline void paintBBOX(const glm::mat4& mvMat,
                        const glm::mat4& mvpMat,
                        const glm::mat4& normMat) const {
    if (_bbox != nullptr && _isVisibleBBOX) {
      _shader->bind();
      _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_MV_MAT, mvMat);
      _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_MVP_MAT, mvpMat);
      _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_NORM_MAT, normMat);
      _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_RENDER_TYPE, getRenderType(false, RenderType::COLOR));

      _bbox->draw();

      _shader->unbind();
    }
  };

  virtual void update() = 0;
  virtual void initVAO() = 0;
  virtual void paintGL(
      const glm::mat4& mvMat,
      const glm::mat4& mvpMat,
      const glm::mat4& lightMat,
      const glm::vec3& lightPos,
      const float& shininess,
      const float& ambientIntensity,
      const glm::vec3& wireFrameColor,
      const float& wireFrameWidth,
      const GLuint& depthTextureId,
      const glm::mat4& lightMvpMat) = 0;
  virtual void drawGL(const int& index) = 0;
  virtual void drawAllGL(const glm::mat4& lightMvpMat) = 0;
};

using Primitives_t = std::shared_ptr<Primitives>;

}  // namespace model
}  // namespace simview