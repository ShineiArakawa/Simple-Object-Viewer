#include <Model/ViewerModel.hpp>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

ViewerModel::ViewerModel() {
  setMaskMode(false);
}

ViewerModel::~ViewerModel() {}

void ViewerModel::initVAO() {
  const int nBackgrounds = getNumBackgrounds();
  for (int iBackground = 0; iBackground < nBackgrounds; iBackground++) {
    getBackground(iBackground)->initVAO();

    if (iBackground % 10000 == 0 || iBackground == nBackgrounds - 1) {
      LOG_INFO("Initialized " + std::to_string(iBackground + 1) + " backgrounds.");
    }
  }

  const int nModels = getNumObjects();
  for (int iModel = 0; iModel < nModels; iModel++) {
    getObject(iModel)->initVAO();

    if (iModel % 10000 == 0 || iModel == nModels - 1) {
      LOG_INFO("Initialized " + std::to_string(iModel + 1) + " models.");
    }
  }
}

void ViewerModel::paintGL(const glm::mat4 &mvMat,        // mvMat
                          const glm::mat4 &mvpMat,       // mvpMat
                          const glm::mat4 &lightMat,     // lightMat
                          const glm::mat4 &lightMvpMat,  // lightMvpMat
                          const GLuint &depthMapId       // depthMapId
) {
  // ================================================================================================
  // Draw static objects first
  // ================================================================================================
  if (_backgroundIDtoDraw >= 0 && _backgroundIDtoDraw < _backgrounds->size()) {
    getBackground(_backgroundIDtoDraw)->update();
    getBackground(_backgroundIDtoDraw)->paintGL(mvMat,                 // mvMat
                                                mvpMat,                // mvpMat
                                                lightMat,              // lightMat
                                                _lightPosition.xyz(),  // lightPos
                                                _shininess,            // shininess
                                                _ambientIntensity,     // ambientIntensity
                                                _wireFrameColor,       // wireFrameColor
                                                _wireFrameWidth,       // wireFrameWidth
                                                depthMapId,            // depthTextureId
                                                lightMvpMat            // lightMvpMat
    );
  }

  // ================================================================================================
  // Draw objects
  // ================================================================================================
  const int &nObjects = getNumObjects();
  for (int iModel = 0; iModel < nObjects; ++iModel) {
    getObject(iModel)->update();
    getObject(iModel)->paintGL(
        mvMat,                 // mvMat
        mvpMat,                // mvpMat
        lightMat,              // lightMat
        _lightPosition.xyz(),  // lightPos
        _shininess,            // shininess
        _ambientIntensity,     // ambientIntensity
        _wireFrameColor,       // wireFrameColor
        _wireFrameWidth,       // wireFrameWidth
        depthMapId,            // depthTextureId
        lightMvpMat            // lightMvpMat
    );
  }
}

void ViewerModel::setAxesConeState(const bool &isShown) {
  if (isShown) {
    // Add
    for (int iModel = 0; iModel < getNumObjects(); iModel++) {
      if (getObject(iModel)->getObjectType() == AxesCone::KEY_MODEL_AXES_CONE) {
        return;
      }
    }

    auto axesCone = std::make_shared<AxesCone>(32, -10.0f, -10.0f, -10.0f, 1.0f);
    axesCone->setName("Axes cone");
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

void ViewerModel::setGridPlaneState(const bool &isShown) {
  if (isShown) {
    // Add
    for (int iModel = 0; iModel < getNumObjects(); iModel++) {
      if (getObject(iModel)->getObjectType() == GridPlane::KEY_MODEL_GRID_PLANE) {
        return;
      }
    }

    auto gridPlane = std::make_shared<GridPlane>(20, glm::vec2(-50.0f, -50.0f), glm::vec2(50.0f, 50.0f));
    gridPlane->setName("XY-Grid");
    addObject(gridPlane);
  } else {
    // Remove
    for (int iModel = 0; iModel < getNumObjects(); iModel++) {
      if (getObject(iModel)->getObjectType() == GridPlane::KEY_MODEL_GRID_PLANE) {
        removeObject(iModel);
      }
    }
  }
}

}  // namespace model
}  // namespace simview