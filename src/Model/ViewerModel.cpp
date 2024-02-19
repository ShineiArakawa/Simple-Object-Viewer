#include <Model/ViewerModel.hpp>

ViewerModel::ViewerModel() {
  setMaskMode(false);
  compileShaders();
}

ViewerModel::~ViewerModel() {}

void ViewerModel::initVAO() {
  int nBackgrounds = getNumBackgrounds();
  for (int iBackground = 0; iBackground < nBackgrounds; iBackground++) {
    getBackground(iBackground)->initVAO();

    if (iBackground % 10000 == 0 || iBackground == nBackgrounds - 1) {
      std::cout << "Initialized " << iBackground + 1 << " backgrounds."
                << std::endl;
    }
  }

  int nModels = getNumObjects();
  for (int iModel = 0; iModel < nModels; iModel++) {
    getObject(iModel)->initVAO();

    if (iModel % 10000 == 0 || iModel == nModels - 1) {
      std::cout << "Initialized " << iModel + 1 << " models." << std::endl;
    }
  }
}

void ViewerModel::paintGL(const glm::mat4& mvpMat) {
  if (_backgroundIDtoDraw >= 0 && _backgroundIDtoDraw < _backgrounds->size()) {
    getBackground(_backgroundIDtoDraw)->update();
    getBackground(_backgroundIDtoDraw)->paintGL(mvpMat);
  }

  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->update();
    getObject(iModel)->paintGL(mvpMat);
  }
}
