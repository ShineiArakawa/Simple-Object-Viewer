#pragma once

#include <Shader/DefaultShaders.hpp>
#include <Shader/Shader.hpp>

class DepthShader : public Shader {
 public:
  DepthShader(const std::string& vertShaderCode,
              const std::string& fragShaderCode);
  ~DepthShader();
};