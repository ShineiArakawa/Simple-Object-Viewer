#include <SimView/Model/ViewerModel.hpp>

namespace simview {
namespace model {

using namespace util;
using namespace shader;

ViewerModel::ViewerModel()
    : Model() {
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

void ViewerModel::paintGL(
    const TransformationContext &transCtx,  // transCtx
    const GLuint &depthMapId                // depthMapId
) {
  const glm::vec3 &lightPosition = _lightPosition.xyz();

  const model::LightingContext lightingCtx(
      lightPosition,     // lightPos
      _shininess,        // shininess
      _ambientIntensity  // ambientIntensity
  );

  const model::RenderingContext renderingCtx(
      _wireFrameColor,  // wireFrameColor
      _wireFrameWidth,  // wireFrameWidth
      depthMapId        // depthTextureId
  );

  // ================================================================================================
  // Draw static objects first
  // ================================================================================================
  if (_backgroundIDtoDraw >= 0 && _backgroundIDtoDraw < _backgrounds->size()) {
    getBackground(_backgroundIDtoDraw)->update();
    getBackground(_backgroundIDtoDraw)->paintGL(transCtx, lightingCtx, renderingCtx);
  }

  // ================================================================================================
  // Draw objects
  // ================================================================================================
  const int &nObjects = getNumObjects();
  for (int iModel = 0; iModel < nObjects; ++iModel) {
    getObject(iModel)->update();
    getObject(iModel)->paintGL(transCtx, lightingCtx, renderingCtx);
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

void ViewerModel::setLightBallState(const bool &isShown) {
  if (isShown) {
    // Add
    for (int iModel = 0; iModel < getNumObjects(); iModel++) {
      if (getObject(iModel)->getObjectType() == LightBall::KEY_MODEL_LIGHT_BALL) {
        return;
      }
    }

    auto lightBall = std::make_shared<LightBall>();
    lightBall->setName("Light");
    addObject(lightBall);
  } else {
    // Remove
    for (int iModel = 0; iModel < getNumObjects(); iModel++) {
      if (getObject(iModel)->getObjectType() == LightBall::KEY_MODEL_LIGHT_BALL) {
        removeObject(iModel);
      }
    }
  }
}

}  // namespace model
}  // namespace simview