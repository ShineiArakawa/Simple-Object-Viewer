#include <Shader/DepthShader.hpp>

DepthShader::DepthShader(const std::string& vertShaderCode, const std::string& fragShaderCode) {
  setShaders(vertShaderCode, fragShaderCode);
  _textureCounterOffset = 31;
}

DepthShader::~DepthShader() = default;