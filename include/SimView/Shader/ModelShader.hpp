#pragma once

#include <SimView/Shader/Shader.hpp>

namespace simview {
namespace shader {

class LineShader : public Shader {
 public:
  LineShader();
  ~LineShader();
};

using LineShader_t = std::shared_ptr<LineShader>;

class ModelShader : public Shader {
 private:
  LineShader_t _lineShader = nullptr;

 public:
  ModelShader(const std::string& vertShaderCode,
              const std::string& fragShaderCode);
  ~ModelShader();

  LineShader_t getLineShader();
};

using ModelShader_t = std::shared_ptr<ModelShader>;

}  // namespace shader
}  // namespace simview