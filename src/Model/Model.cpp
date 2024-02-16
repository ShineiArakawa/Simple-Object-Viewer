#include <Model/Model.hpp>

Model::~Model() {}

void Model::compileShaders() {
  std::string vertShaderCode;
  std::string fragShaderCode;

  if (_vertShaderPath == nullptr) {
    std::cout << "Use default vertex shader." << std::endl;
    vertShaderCode = DefaultShaders::VERT_SHADER;
  } else {
    vertShaderCode = ShaderCompiler::readCodesFromFIle(*_vertShaderPath);
  }

  if (_fragShaderPath == nullptr) {
    std::cout << "Use default fragment shader." << std::endl;
    fragShaderCode = DefaultShaders::FRAG_SHADER;
  } else {
    fragShaderCode = ShaderCompiler::readCodesFromFIle(*_fragShaderPath);
  }

  const GLuint shaderID = ShaderCompiler::buildShaderProgram(vertShaderCode, fragShaderCode);

  for (t_object object : *_objects) {
    object->setShader(shaderID);
  }
  for (t_object backgrounds : *_backgrounds) {
    backgrounds->setShader(shaderID);
  }
}

void Model::setMaskMode(const bool maskMode) {
  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->setMaskMode(maskMode);
  }
}

void Model::setRenderType(const Primitives::RenderType renderType) {
  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->setRenderType(renderType);
  }
}

void Model::resetRenderType() {
  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->resetRenderType();
  }
}
