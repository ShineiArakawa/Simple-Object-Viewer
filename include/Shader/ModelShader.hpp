#pragma once

#include <Shader/Shader.hpp>

namespace oglt {
namespace shader {
class ModelShader : public Shader {
 public:
  ModelShader(const std::string& vertShaderCode,
              const std::string& fragShaderCode);
  ~ModelShader();
};
}  // namespace shader
}  // namespace oglt