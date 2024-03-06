#include <Shader/Shader.hpp>

namespace simview {
namespace shader {

Shader::~Shader() = default;

void Shader::setShaders(const std::string& vertShaderCode, const std::string& fragShaderCode) {
  LOG_INFO("Start compiling shaders");
  _shaderID = ShaderCompiler::buildShaderProgram(vertShaderCode, fragShaderCode);
  LOG_INFO("Finish compiling shaders");
}

void Shader::bind(const bool& disableDepthTest) {
  _textureCounter = _textureCounterOffset;

  // Enable shader program
  glUseProgram(_shaderID);

  if (disableDepthTest) {
    glDisable(GL_DEPTH_TEST);
  }
}

void Shader::unbind() {
  glEnable(GL_DEPTH_TEST);

  // Unbind all texture
  for (int iTexture = 0; iTexture < _textureCounter - _textureCounterOffset; iTexture++) {
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  _textureCounter = _textureCounterOffset;

  // Disable shader program
  glUseProgram(0);
}

void Shader::setUniformVariable(const std::string& name, const glm::mat4& matrix) {
  const GLuint& uid = glGetUniformLocation(_shaderID, name.c_str());
  glUniformMatrix4fv(uid, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setUniformVariable(const std::string& name, const glm::vec3& vec) {
  const GLuint& uid = glGetUniformLocation(_shaderID, name.c_str());
  glUniform3fv(uid, 1, glm::value_ptr(vec));
}

void Shader::setUniformVariable(const std::string& name, const float& value) {
  const GLuint& uid = glGetUniformLocation(_shaderID, name.c_str());
  glUniform1f(uid, value);
}

void Shader::setUniformVariable(const std::string& name, const int& value) {
  const GLuint& uid = glGetUniformLocation(_shaderID, name.c_str());
  glUniform1f(uid, (float)value);
}

void Shader::setUniformVariable(const std::string& name, const bool& value) {
  const GLuint& uid = glGetUniformLocation(_shaderID, name.c_str());
  glUniform1f(uid, (float)value);
}

void Shader::setUniformTexture(const std::string& name, const GLuint& textureId) {
  glActiveTexture(GL_TEXTURE_IDS[_textureCounter]);
  glBindTexture(GL_TEXTURE_2D, textureId);
  const GLuint& uid = glGetUniformLocation(_shaderID, name.c_str());
  glUniform1i(uid, _textureCounter);
  _textureCounter++;
}

}  // namespace shader
}  // namespace simview