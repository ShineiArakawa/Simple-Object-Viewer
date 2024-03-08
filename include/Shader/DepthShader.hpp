#pragma once

#include <Shader/DefaultShaders.hpp>
#include <Shader/Shader.hpp>

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