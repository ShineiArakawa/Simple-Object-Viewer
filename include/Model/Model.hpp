#pragma once
#ifndef _MODEL_H_
#define _MODEL_H_

#include <Model/Background.hpp>
#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Model {
  using t_object = std::shared_ptr<Primitives>;
  using t_objects = std::shared_ptr<std::vector<t_object>>;

 private:
  t_objects _objects = std::make_shared<std::vector<t_object>>();
  std::array<float, 4> _backgroundColor;
  std::shared_ptr<Background> _background = nullptr;
  float _time = 0.0f;

 public:
  Model();
  ~Model();

  void initVAO();
  void paintGL(const glm::mat4& mvpMat);

  void setBackground(std::shared_ptr<Background> background) {
    _background = background;
  };
  void addObject(t_object object) { _objects->push_back(object); };
  void tick(float time) { _time += time; };
  t_objects getObjects() { return _objects; };
  t_object getObject(const int index) { return (*_objects)[index]; };
  int getNumObjects() { return (int)_objects->size(); };
  void setMaskMode(const bool maskMode);
  void setRenderType(const Primitives::RenderType renderType);
  void resetRenderType();
  void setBackgroundColor(const float r, const float g, const float b,
                          const float a) {
    _backgroundColor[0] = r;
    _backgroundColor[1] = g;
    _backgroundColor[2] = b;
    _backgroundColor[3] = a;
  }
  void setBackgroundColor(std::array<float, 4> rgba) {
    _backgroundColor = rgba;
  }
  std::array<float, 4> getBackgroundColor() { return _backgroundColor; };
};
#endif