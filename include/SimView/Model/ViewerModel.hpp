#pragma once

#include <SimView/Model/AxesCone.hpp>
#include <SimView/Model/Background.hpp>
#include <SimView/Model/GridPlane.hpp>
#include <SimView/Model/LightBall.hpp>
#include <SimView/Model/Model.hpp>
#include <SimView/Model/Primitives.hpp>
#include <SimView/OpenGL.hpp>
#include <SimView/Util/Logging.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace simview {
namespace model {

class ViewerModel : public Model {
 public:
  ViewerModel();
  ~ViewerModel();

  void initVAO() override;

  void paintGL(
      const TransformationContext &transCtx,  // transCtx
      const GLuint &depthMapId                // depthMapId
      ) override;

  void tick(float time) override { _time += time; };

  void setAxesConeState(const bool &isShown);

  void setGridPlaneState(const bool &isShown);

  void setLightBallState(const bool &isShown);
};

using ViewerModel_t = std::shared_ptr<ViewerModel>;

}  // namespace model
}  // namespace simview
