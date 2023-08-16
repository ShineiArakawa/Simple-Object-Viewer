#include <Model/Model.hpp>

Model::~Model() {}

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
