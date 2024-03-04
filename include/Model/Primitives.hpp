#pragma once
#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

#define _USE_MATH_DEFINES
#include <math.h>

#include <OpenGL.hpp>
#include <Shader/DefaultShaders.hpp>
#include <Shader/DepthShader.hpp>
#include <Shader/ModelShader.hpp>
#include <Util/Logging.hpp>
#include <array>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

inline static const std::array<glm::vec3, 3> BARY_CENTER = {
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f)};

struct Vertex {
  Vertex() = default;
  Vertex(
      const glm::vec3& position_,
      const glm::vec3& color_,
      const glm::vec3& normal_,
      const glm::vec3& bary_,
      const glm::vec2& uv_,
      const float id_)
      : position(position_), color(color_), normal(normal_), bary(bary_), uv(uv_), id(id_){};

  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 normal;
  glm::vec3 bary;
  glm::vec2 uv;
  float id;
};

struct MaterialGroup {
  MaterialGroup()
      : vertices(std::shared_ptr<std::vector<Vertex>>(new std::vector<Vertex>)),
        indices(std::shared_ptr<std::vector<uint32_t>>(new std::vector<uint32_t>)){};

  std::shared_ptr<std::vector<Vertex>> vertices = nullptr;
  std::shared_ptr<std::vector<uint32_t>> indices = nullptr;

  float shininess;          // Ns
  glm::vec3 ambientColor;   // Ka
  glm::vec3 diffuseColor;   // Kd
  glm::vec3 specularColor;  // Ks
  glm::vec3 emissiveColor;  // Ke
  float refractiveIndex;    // Ni
  float opacity;            // d

  std::string ambientTexturePath;             // map_Ka
  std::string diffuseTexturePath;             // map_Kd
  std::string specularTexturePath;            // map_Ks
  std::string specularReflectionTexturePath;  // map_Ns
  std::string opacityTexturePath;             // map_d
  std::string roughnessTexturePath;           // disp
  std::string bumpTexturePath;                // map_Bump
  std::string reflectionDiffuseTexturePath;   // refl
};

using MaterialGroup_t = std::shared_ptr<MaterialGroup>;
using MaterialGroups_t = std::shared_ptr<std::vector<MaterialGroup_t>>;

class Primitives {
  // ==================================================================================================
  // Type defines
  // ==================================================================================================
 public:
  using ModelShader_t = std::shared_ptr<ModelShader>;
  using DepthShader_t = std::shared_ptr<DepthShader>;

  template <class dtype>
  using vec4_t = std::array<dtype, 4>;
  using vec4f_t = vec4_t<float>;

  template <class dtype>
  using vec3_t = std::array<dtype, 3>;
  using vec3f_t = vec3_t<float>;

  template <class dtype>
  using vec2_t = std::array<dtype, 2>;
  using vec2f_t = vec2_t<float>;

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
  ModelShader_t _shader = nullptr;
  DepthShader_t _depthShader = nullptr;
  Primitives::RenderType _defaultRenderType = Primitives::RenderType::COLOR;
  Primitives::RenderType _renderType = Primitives::RenderType::COLOR;
  Primitives::WireFrameMode _wireFrameMode = Primitives::WireFrameMode::OFF;
  bool _maskMode = false;
  bool _isVisible = true;
  bool _isEnabledNormalMap = false;
  bool _isEnabledShadowMapping = false;
  glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 _vecocity = glm::vec3(0.0f, 0.0f, 0.0f);

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
  void setName(std::string name) { _name = name; };
  void setModelShader(ModelShader_t shader) { _shader = shader; };
  void setDepthShader(DepthShader_t shader) { _depthShader = shader; };
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
  void setDefaultRenderType(const Primitives::RenderType& renderType) {
    _defaultRenderType = renderType;
    _renderType = renderType;
  };

  void resetRenderType() { _renderType = _defaultRenderType; };

  void setRenderType(const Primitives::RenderType& renderType) { _renderType = renderType; };

  float getRenderType() {
    return getRenderType(_maskMode, _renderType);
  };

  void setWireFrameMode(const Primitives::WireFrameMode& wireFrameMode) { _wireFrameMode = wireFrameMode; };

  float getWireFrameMode() const {
    return getWireFrameMode(_wireFrameMode);
  };

  inline static float getRenderType(const bool& maskMode, const Primitives::RenderType& renderType) {
    float renderTypeValue = 0.0f;

    if (maskMode) {
      renderTypeValue = -2.0f;
    } else if (renderType == Primitives::RenderType::NORMAL) {
      renderTypeValue = -1.0f;
    } else if (renderType == Primitives::RenderType::COLOR) {
      renderTypeValue = 0.0f;
    } else if (renderType == Primitives::RenderType::TEXTURE) {
      renderTypeValue = 1.0f;
    } else if (renderType == Primitives::RenderType::VERT_NORMAL) {
      renderTypeValue = -3.0f;
    } else if (renderType == Primitives::RenderType::SHADE) {
      renderTypeValue = 2.0f;
    } else if (renderType == Primitives::RenderType::SHADE_TEXTURE) {
      renderTypeValue = 3.0f;
    } else if (renderType == Primitives::RenderType::MATERIAL) {
      renderTypeValue = 4.0f;
    }

    return renderTypeValue;
  };

  inline static float getWireFrameMode(const Primitives::WireFrameMode& wireFrameType) {
    float wireFrameTypeValue = 0.0f;

    if (wireFrameType == Primitives::WireFrameMode::ON) {
      wireFrameTypeValue = 1.0f;
    } else if (wireFrameType == Primitives::WireFrameMode::ONLY) {
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
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_MV_MAT, mvMat);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_MVP_MAT, mvpMat);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_NORM_MAT, normMat);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_LIGHT_MAT, lightMat);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_LIGHT_POS, lightPos);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_SHININESS, shininess);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_AMBIENT_INTENSITY, ambientIntensity);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_AMBIENT_COLOR, ambientColor);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_DIFFUSE_COLOR, diffuseColor);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_SPECULAR_COLOR, specularColor);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_WIRE_FRAME_MODE, wireFrameMode);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_WIRE_FRAME_COLOR, wireFrameColor);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_WIRE_FRAME_WIDTH, wireFrameWidth);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_RENDER_TYPE, renderType);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_BUMP_MAP, isEnabledNormalMap);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_LIGHT_MVP_MAT, lightMvpMat);
    _shader->setUniformVariable(DefaultModelShader::UNIFORM_NAME_SHADOW_MAPPING, _isEnabledShadowMapping);

    _shader->setUniformTexture(DefaultModelShader::UNIFORM_NAME_DEPTH_TEXTURE, depthTextureId);
  };

  inline void unbindShader() const {
    _shader->unbind();
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
#endif
