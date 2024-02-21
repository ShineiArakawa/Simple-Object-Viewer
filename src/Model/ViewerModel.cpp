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

void ViewerModel::paintGL(const glm::mat4 &mvMat, const glm::mat4 &mvpMat, const glm::mat4 &normMat, const glm::mat4 &lightMat) {
  const glm::vec3 lightPosition = glm::vec3(_lightPosition[0], _lightPosition[1], _lightPosition[2]);
  const glm::vec3 wireFrameColor = glm::vec3(_wireFrameColor[0], _wireFrameColor[1], _wireFrameColor[2]);

  if (_backgroundIDtoDraw >= 0 && _backgroundIDtoDraw < _backgrounds->size()) {
    getBackground(_backgroundIDtoDraw)->update();
    getBackground(_backgroundIDtoDraw)->paintGL(mvMat, mvpMat, normMat, lightMat, lightPosition, _shininess, _ambientIntensity, wireFrameColor, _wireFrameWidth);
  }

  for (int iModel = 0; iModel < getNumObjects(); iModel++) {
    getObject(iModel)->update();
    getObject(iModel)->paintGL(
        mvMat,
        mvpMat,
        normMat,
        lightMat,
        lightPosition,
        _shininess,
        _ambientIntensity,
        wireFrameColor,
        _wireFrameWidth);
  }
}

void ViewerModel::setAxesConeState(const bool isShown) {
  if (isShown) {
    // Add
    for (int iModel = 0; iModel < getNumObjects(); iModel++) {
      if (getObject(iModel)->getObjectType() == AxesCone::KEY_MODEL_AXES_CONE) {
        return;
      }
    }

    auto axesCone = std::make_shared<AxesCone>(32, -10.0f, -10.0f, -10.0f, 1.0f);
    axesCone->setName("Axes cone");
    axesCone->setShader(_shaderID);
    axesCone->initVAO();
    addObject(axesCone);
  } else {
    // Remove
    for (int iModel = 0; iModel < getNumObjects(); iModel++) {
      if (getObject(iModel)->getObjectType() == AxesCone::KEY_MODEL_AXES_CONE) {
        removeObject(iModel);
      }
    }
  }
}
