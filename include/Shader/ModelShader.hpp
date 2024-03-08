#pragma once

#include <Shader/Shader.hpp>

namespace simview {
namespace shader {

class ModelShader : public Shader {
 public:
  ModelShader(const std::string& vertShaderCode,
              const std::string& fragShaderCode);
  ~ModelShader();
};

using ModelShader_t = std::shared_ptr<ModelShader>;

}  // namespace shader
}  // namespace simview