#include <SimView/Model/Model.hpp>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

Model::~Model() {}

void Model::compileShaders(const bool& isQuad) {
  {
    std::string modelVertShaderCode;
    std::string modelFragShaderCode;

    if (isQuad) {
      modelVertShaderCode = DefaultDepthQuadShader::VERT_SHADER;
      modelFragShaderCode = DefaultDepthQuadShader::FRAG_SHADER;
    } else {
      if (_modelVertMShaderPath == nullptr) {
        LOG_INFO("Use default vertex model shader.");
        modelVertShaderCode = DefaultModelShader::VERT_SHADER;
      } else {
        modelVertShaderCode = ShaderCompiler::readCodesFromFile(*_modelVertMShaderPath);
      }

      if (_modelFragShaderPath == nullptr) {
        LOG_INFO("Use default fragment model shader.");
        modelFragShaderCode = DefaultModelShader::FRAG_SHADER;
      } else {
        modelFragShaderCode = ShaderCompiler::readCodesFromFile(*_modelFragShaderPath);
      }
    }

    _shader = std::make_shared<ModelShader>(modelVertShaderCode, modelFragShaderCode);
  }

  {
    std::string depthVertShaderCode;
    std::string depthFragShaderCode;

    if (_depthVertMShaderPath == nullptr) {
      LOG_INFO("Use default vertex depth shader.");
      depthVertShaderCode = DefaultDepthShader::VERT_SHADER;
    } else {
      depthVertShaderCode = ShaderCompiler::readCodesFromFile(*_depthVertMShaderPath);
    }

    if (_depthFragShaderPath == nullptr) {
      LOG_INFO("Use default fragment depth shader.");
      depthFragShaderCode = DefaultDepthShader::FRAG_SHADER;
    } else {
      depthFragShaderCode = ShaderCompiler::readCodesFromFile(*_depthFragShaderPath);
    }

    _depthShader = std::make_shared<DepthShader>(depthVertShaderCode, depthFragShaderCode);
  }
}

void Model::setModelShader(ModelShader_t shader) {
  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->setModelShader(shader);
  }
}

void Model::setDepthShader(DepthShader_t shader) {
  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->setDepthShader(shader);
  }
}

void Model::setMaskMode(const bool maskMode) {
  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->setMaskMode(maskMode);
  }
}

void Model::setRenderType(const Primitive::RenderType renderType) {
  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->setRenderType(renderType);
  }
}

void Model::setWireFrameMode(Primitive::WireFrameMode wireFrameMode) {
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

void Model::setIsEnabledShadowMapping(bool isEnabledShadowMapping) {
  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->setIsEnabledShadowMapping(isEnabledShadowMapping);
  }
}

void Model::setIsVisibleBBOX(bool isVisible) {
  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->setVisibleBBOX(isVisible);
  }
}

}  // namespace model
}  // namespace simview