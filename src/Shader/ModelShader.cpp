#include <Shader/ModelShader.hpp>

ModelShader::ModelShader(const std::string& vertShaderCode, const std::string& fragShaderCode) {
  setShaders(vertShaderCode, fragShaderCode);
  _textureCounterOffset = 0;
}

ModelShader::~ModelShader() = default;
