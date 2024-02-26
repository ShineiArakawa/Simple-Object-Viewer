#include <Model/Model.hpp>

Model::~Model() {}

void Model::compileShaders() {
  std::string vertShaderCode;
  std::string fragShaderCode;

  if (_vertShaderPath == nullptr) {
    LOG_INFO("Use default vertex shader.");
    vertShaderCode = DefaultShaders::VERT_SHADER;
  } else {
    vertShaderCode = ShaderCompiler::readCodesFromFile(*_vertShaderPath);
  }

  if (_fragShaderPath == nullptr) {
    LOG_INFO("Use default fragment shader.");
    fragShaderCode = DefaultShaders::FRAG_SHADER;
  } else {
    fragShaderCode = ShaderCompiler::readCodesFromFile(*_fragShaderPath);
  }

  LOG_INFO("Start compiling shaders");

  _shaderID = ShaderCompiler::buildShaderProgram(vertShaderCode, fragShaderCode);

  for (t_object object : *_objects) {
    object->setShader(_shaderID);
  }
  for (t_object backgrounds : *_backgrounds) {
    backgrounds->setShader(_shaderID);
  }

  LOG_INFO("Finish compiling shaders");
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

void Model::setWireFrameMode(Primitives::WireFrameMode wireFrameMode) {
  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->setWireFrameMode(wireFrameMode);
  }
}

void Model::resetRenderType() {
  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->resetRenderType();
  }
}

void Model::setIsEnabledNormalMap(bool isEnabledNormalMap) {
  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->setIsEnabledNormalMap(isEnabledNormalMap);
  }
}
