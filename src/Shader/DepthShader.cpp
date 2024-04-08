#include <SimView/Shader/DepthShader.hpp>

namespace simview {
namespace shader {

DepthShader::DepthShader(const std::string& vertShaderCode, const std::string& fragShaderCode) {
  setShaders(vertShaderCode, fragShaderCode);
  _textureCounterOffset = 31;
}

DepthShader::~DepthShader() = default;

}  // namespace shader
}  // namespace simview