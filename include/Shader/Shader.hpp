#pragma once

#include <OpenGL.hpp>
#include <Shader/ShaderCompiler.hpp>
#include <Util/Logging.hpp>
#include <string>

namespace simview {
namespace shader {

class Shader {
 private:
  inline const static int MAX_NUM_TEXTURES = 32;
  inline const static int GL_TEXTURE_IDS[MAX_NUM_TEXTURES] = {
      GL_TEXTURE0,
      GL_TEXTURE1,
      GL_TEXTURE2,
      GL_TEXTURE3,
      GL_TEXTURE4,
      GL_TEXTURE5,
      GL_TEXTURE6,
      GL_TEXTURE7,
      GL_TEXTURE8,
      GL_TEXTURE9,
      GL_TEXTURE10,
      GL_TEXTURE11,
      GL_TEXTURE12,
      GL_TEXTURE13,
      GL_TEXTURE14,
      GL_TEXTURE15,
      GL_TEXTURE16,
      GL_TEXTURE17,
      GL_TEXTURE18,
      GL_TEXTURE19,
      GL_TEXTURE20,
      GL_TEXTURE21,
      GL_TEXTURE22,
      GL_TEXTURE23,
      GL_TEXTURE24,
      GL_TEXTURE25,
      GL_TEXTURE26,
      GL_TEXTURE27,
      GL_TEXTURE28,
      GL_TEXTURE29,
      GL_TEXTURE30,
      GL_TEXTURE31};

  GLuint _shaderID;
  int _textureCounter = 0;

 protected:
  int _textureCounterOffset;

 public:
  // Nothing

 private:
  // Nothing

 protected:
  void setShaders(const std::string& vertShaderCode, const std::string& fragShaderCode);
  virtual ~Shader();

 public:
  void bind(const bool& disableDepthTest = false);
  void unbind();

  void setUniformVariable(const std::string& name, const glm::mat4& matrix);
  void setUniformVariable(const std::string& name, const glm::vec3& vec);
  void setUniformVariable(const std::string& name, const float& value);
  void setUniformVariable(const std::string& name, const int& value);
  void setUniformVariable(const std::string& name, const bool& value);
  void setUniformTexture(const std::string& name, const GLuint& textureId);
};

using Shader_t = std::shared_ptr<Shader>;

}  // namespace shader
}  // namespace simview