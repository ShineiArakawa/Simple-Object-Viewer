#pragma once

#include <SimView/Model/AxisAlignedBoundingBox.hpp>
#include <SimView/Model/RenderingContext.hpp>
#include <SimView/Model/WireFrame.hpp>
#include <SimView/OpenGL.hpp>
#include <SimView/Shader/DefaultShaders.hpp>
#include <SimView/Shader/DepthShader.hpp>
#include <SimView/Shader/ModelShader.hpp>
#include <SimView/Util/DataStructure.hpp>
#include <SimView/Util/Logging.hpp>
#include <SimView/Util/Math.hpp>
#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace simview {
namespace model {

class Primitive {
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

  static RenderType getRenderType(const std::string& string) {
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
  bool _isVisibleBBOX = false;
  glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 _vecocity = glm::vec3(0.0f, 0.0f, 0.0f);
  AxisAlignedBoundingBox_t _bbox = nullptr;
  WireFrame_t _wireFrame = nullptr;

  int _indexBufferSize;

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
  Primitive()
      : _name(),
        _shader(),
        _depthShader(),
        _defaultRenderType(RenderType::COLOR),
        _renderType(_defaultRenderType),
        _wireFrameMode(WireFrameMode::OFF),
        _maskMode(false),
        _isVisible(true),
        _isEnabledNormalMap(false),
        _isEnabledShadowMapping(false),
        _isVisibleBBOX(false),
        _position(0.0f),
        _vecocity(0.0f),
        _bbox(),
        _wireFrame(),
        _indexBufferSize() {
  }

  virtual ~Primitive() = default;

  void setMaskMode(bool maskMode) {
    _maskMode = maskMode;
  };

  void setVisible(bool isVisible) {
    _isVisible = isVisible;
  };

  void setIsEnabledNormalMap(bool isEnabledNormalMap) {
    _isEnabledNormalMap = isEnabledNormalMap;
  };

  void setIsEnabledShadowMapping(bool isEnabledShadowMapping) {
    _isEnabledShadowMapping = isEnabledShadowMapping;
  };

  void setVisibleBBOX(bool isVisibleBBOX) {
    _isVisibleBBOX = isVisibleBBOX;
  };

  void setName(std::string name) {
    _name = name;
  };

  void setModelShader(shader::ModelShader_t shader) {
    _shader = shader;
  };

  void setDepthShader(shader::DepthShader_t shader) {
    _depthShader = shader;
  };

  void setPosition(const glm::vec3& position) {
    _position = position;
  };

  void setPosition(const float& x, const float& y, const float& z) {
    _position.x = x;
    _position.y = y;
    _position.z = z;
  };

  void setVecocity(const glm::vec3& vecocity) {
    _vecocity = vecocity;
  };

  void forward(const float& deltaT) {
    _position = _position + deltaT * _vecocity;
  };

  std::string getName() const {
    return _name;
  };

  glm::vec3 getPosition() const {
    return _position;
  };

  glm::vec3 getVecocity() const {
    return _vecocity;
  };

  bool* getPointerToIsVisible() {
    return &_isVisible;
  };

  virtual std::string getObjectType() = 0;

  // ==================================================================================================
  // Rendering options
  // ==================================================================================================
  void setDefaultRenderType(const RenderType& renderType) {
    _defaultRenderType = renderType;
    _renderType = renderType;
  };

  void resetRenderType() {
    _renderType = _defaultRenderType;
  };

  void setRenderType(const RenderType& renderType) {
    _renderType = renderType;
  };

  float getRenderType() const {
    return getRenderType(_maskMode, _renderType);
  };

  void setWireFrameMode(const WireFrameMode& wireFrameMode) {
    _wireFrameMode = wireFrameMode;
  };

  WireFrameMode getWireFrameMode() {
    return _wireFrameMode;
  }

  inline static float getRenderType(const bool& maskMode,
                                    const RenderType& renderType) {
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

  // ==================================================================================================
  // Rendering
  // ==================================================================================================

  /// @brief Bind shader program and attach uniform variables
  /// @param mvMat Model view matrix
  /// @param mvpMat Model view projection matrix
  /// @param normMat Normal matrix
  /// @param lightMat Light model view matrix
  /// @param lightPos Light position
  /// @param shininess Specular exponent
  /// @param ambientIntensity Ambient factor
  /// @param ambientColor Ambient color
  /// @param diffuseColor Diffuse color
  /// @param specularColor Specular color
  /// @param renderType Rendering mode
  /// @param wireFrameMode Wire frame mode
  /// @param wireFrameColor Wire frame color
  /// @param wireFrameWidth Wire frame width
  /// @param depthTextureId Depth texture map id
  /// @param lightMvpMat Light model view projection matrix
  /// @param isEnabledShadowMapping Whether to render shadows using depth map
  /// @param disableDepthTest Disable depth during rendering
  /// @param isEnabledNormalMap Whether to enable bump mapping
  inline void bindShader(
      const glm::mat4& mvMat,              // mvMat
      const glm::mat4& mvpMat,             // mvpMat
      const glm::mat4& normMat,            // normMat
      const glm::mat4& lightMat,           // lightMat
      const glm::vec3& lightPos,           // lightPos
      const float& shininess,              // shininess
      const float& ambientIntensity,       // ambientIntensity
      const glm::vec3& ambientColor,       // ambientColor
      const glm::vec3& diffuseColor,       // diffuseColor
      const glm::vec3& specularColor,      // specularColor
      const float& renderType,             // renderType
      const glm::vec3& wireFrameColor,     // wireFrameColor
      const float& wireFrameWidth,         // wireFrameWidth
      const GLuint& depthTextureId,        // depthTextureId
      const glm::mat4& lightMvpMat,        // lightMvpMat
      const bool& isEnabledShadowMapping,  // isEnabledShadowMapping
      const bool& disableDepthTest,        // disableDepthTest
      const bool& isEnabledNormalMap       // isEnabledNormalMap
  ) const {
    // NOTE: Disable depth test for background draw
    _shader->bind(disableDepthTest);

    // ==================================================================================================
    // Transfer uniform variables
    // ==================================================================================================
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_MV_MAT, mvMat);                        // mvMat
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_MVP_MAT, mvpMat);                      // mvpMat
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_NORM_MAT, normMat);                    // normMat
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_LIGHT_MAT, lightMat);                  // lightMat
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_LIGHT_POS, lightPos);                  // lightPos
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_SHININESS, shininess);                 // shininess
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_AMBIENT_INTENSITY, ambientIntensity);  // ambientIntensity
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_AMBIENT_COLOR, ambientColor);          // ambientColor
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_DIFFUSE_COLOR, diffuseColor);          // diffuseColor
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_SPECULAR_COLOR, specularColor);        // specularColor
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_RENDER_TYPE, renderType);              // renderType
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_BUMP_MAP, isEnabledNormalMap);         // isEnabledNormalMap
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_LIGHT_MVP_MAT, lightMvpMat);           // lightMvpMat

    // ==================================================================================================
    // Transfer uniform variables for shadow mapping
    // ==================================================================================================
    _shader->setUniformVariable(shader::DefaultModelShader::UNIFORM_NAME_SHADOW_MAPPING, isEnabledShadowMapping);  // isEnabledShadowMapping
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

  inline void paintWireFrame(const glm::mat4& mvpMat,
                             const glm::vec3& color,
                             const float& width) const {
    if (_wireFrame != nullptr && (_wireFrameMode == WireFrameMode::ON || _wireFrameMode == WireFrameMode::ONLY)) {
      _shader->getLineShader()->bind();
      _shader->getLineShader()->setUniformVariable(shader::DefaultLineShader::UNIFORM_NAME_MVP_MAT, mvpMat);
      _shader->getLineShader()->setUniformVariable(shader::DefaultLineShader::UNIFORM_NAME_LINE_COLOER, color);

      _wireFrame->draw(width);

      _shader->getLineShader()->unbind();
    }
  };

  virtual void update() = 0;
  virtual void initVAO() = 0;
  virtual void paintGL(
      const TransformationContext& transCtx,  // transCtx
      const LightingContext& lightingCtx,     // lightingCtx
      const RenderingContext& renderingCtx    // renderingCtx
      ) = 0;
  virtual void drawGL(const int& index) = 0;
  virtual void drawAllGL(const glm::mat4& lightMvpMat) = 0;
};

using Primitive_t = std::shared_ptr<Primitive>;

}  // namespace model
}  // namespace simview