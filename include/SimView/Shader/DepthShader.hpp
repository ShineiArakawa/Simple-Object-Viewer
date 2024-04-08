#pragma once

#include <SimView/Shader/DefaultShaders.hpp>
#include <SimView/Shader/Shader.hpp>

namespace simview {
namespace shader {

class DepthShader : public Shader {
 public:
  DepthShader(const std::string& vertShaderCode,
              const std::string& fragShaderCode);
  ~DepthShader();
};

using DepthShader_t = std::shared_ptr<DepthShader>;

}  // namespace shader
}  // namespace simview