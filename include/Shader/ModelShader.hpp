#pragma once

#include <Shader/Shader.hpp>

class ModelShader : public Shader {
 public:
  ModelShader(const std::string& vertShaderCode,
              const std::string& fragShaderCode);
  ~ModelShader();
};