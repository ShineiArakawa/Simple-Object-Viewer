#include <SimView/Shader/DefaultShaders.hpp>
#include <SimView/Shader/ModelShader.hpp>

namespace simview {
namespace shader {

LineShader::LineShader() {
  setShaders(DefaultLineShader::VERT_SHADER,
             DefaultLineShader::FRAG_SHADER);
}

LineShader::~LineShader() = default;

ModelShader::ModelShader(const std::string& vertShaderCode, const std::string& fragShaderCode) {
  setShaders(vertShaderCode, fragShaderCode);
  _textureCounterOffset = 0;

  // Line shader
  _lineShader = std::make_shared<LineShader>();
}

ModelShader::~ModelShader() = default;

LineShader_t ModelShader::getLineShader() {
  return _lineShader;
}

}  // namespace shader
}  // namespace simview