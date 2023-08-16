#pragma once
#ifndef _VIEWER_MODEL_H_
#define _VIEWER_MODEL_H_

#include <Model/Background.hpp>
#include <Model/Model.hpp>
#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class ViewerModel : public Model {
 public:
  ViewerModel();
  ~ViewerModel();

  void initVAO();
  void paintGL(const glm::mat4& mvpMat);
  void tick(float time) { _time += time; };
};
#endif