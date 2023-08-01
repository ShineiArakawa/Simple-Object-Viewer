#include <Model/Model.hpp>

Model::Model(/* args */) {}

Model::~Model() {}

void Model::initVAO() {
  std::cout << "Model::initVAO called." << std::endl;
  if (_background != nullptr) {
    _background->initVAO();
  }

  int nModels = getNumObjects();
  for (int iModel = 0; iModel < nModels; iModel++) {
    getObject(iModel)->initVAO();

    if (iModel % 10000 == 0 || iModel == nModels - 1) {
      std::cout << "Initialized " << iModel + 1 << " models." << std::endl;
    }
  }
}

void Model::paintGL(const glm::mat4& mvpMat) {
  if (_background != nullptr) {
    _background->update();
    _background->paintGL(mvpMat);
  }

  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->update();
    getObject(iModel)->paintGL(mvpMat);
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
