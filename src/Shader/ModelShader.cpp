#include <Shader/ModelShader.hpp>

namespace oglt {
namespace shader {

ModelShader::ModelShader(const std::string& vertShaderCode, const std::string& fragShaderCode) {
  setShaders(vertShaderCode, fragShaderCode);
  _textureCounterOffset = 0;
}

ModelShader::~ModelShader() = default;

}  // namespace shader
}  // namespace oglt