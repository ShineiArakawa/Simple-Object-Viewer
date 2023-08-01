#pragma once
#ifndef _MODEL_H_
#define _MODEL_H_

#include <Model/Background.hpp>
#include <Model/Primitives.hpp>
#include <OpenGL.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Model {
  using t_object = std::shared_ptr<Primitives>;
  using t_objects = std::shared_ptr<std::vector<t_object>>;

 private:
  t_objects _objects = std::make_shared<std::vector<t_object>>();
  std::shared_ptr<Background> _background = nullptr;
  float _time = 0.0f;

 public:
  Model();
  ~Model();

  void initVAO();
  void paintGL(const glm::mat4& mvpMat);

  void setBackground(std::shared_ptr<Background> background) { _background = background; };
  void addObject(t_object object) { _objects->push_back(object); };
  void tick(float time) { _time += time; };
  t_objects getObjects() { return _objects; };
  t_object getObject(const int index) { return (*_objects)[index]; };
  int getNumObjects() { return (int)_objects->size(); };
  void setMaskMode(const bool maskMode);
  void setRenderType(const Primitives::RenderType renderType);
  void resetRenderType();
};
#endif