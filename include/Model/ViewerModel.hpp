#pragma once
#ifndef _VIEWER_MODEL_H_
#define _VIEWER_MODEL_H_

#include <Model/AxesCone.hpp>
#include <Model/Background.hpp>
#include <Model/Model.hpp>
#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <Util/Logging.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class ViewerModel : public Model {
 public:
  ViewerModel();
  ~ViewerModel();

  void initVAO() override;

  void paintGL(const glm::mat4 &mvMat,
               const glm::mat4 &mvpMat,
               const glm::mat4 &lightMat,
               const glm::mat4 &lightMvpMat,
               const GLuint &depthMapId = 0) override;

  void tick(float time) override { _time += time; };

  void setAxesConeState(const bool isShown);
};
#endif