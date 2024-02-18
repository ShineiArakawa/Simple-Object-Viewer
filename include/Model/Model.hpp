#pragma once

#include <Model/Background.hpp>
#include <Model/Primitives.hpp>
#include <Model/ShaderCompiler.hpp>
#include <OpenGL.hpp>
#include <Shaders/DefaultShaders.hpp>
#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Model {
 public:
  using t_object = std::shared_ptr<Primitives>;
  using t_objects = std::shared_ptr<std::vector<t_object>>;
  using t_string = std::shared_ptr<std::string>;

 private:
  // Nothing

 protected:
  t_objects _objects = std::make_shared<std::vector<t_object>>();
  t_objects _backgrounds = std::make_shared<std::vector<t_object>>();
  std::array<float, 4> _backgroundColor = {0.0f, 0.0f, 0.0f, 1.0f};

  t_string _vertShaderPath = nullptr;
  t_string _fragShaderPath = nullptr;

  int _backgroundIDtoDraw = 0;
  float _time = 0.0f;
  GLuint _shaderID;

 public:
  // clang-format off
  inline static const std::string KEY_MODEL = "Model";
  inline static const std::string KEY_BACKGROUND = "Background";
  inline static const std::string KEY_BACKGROUND_COLOR = "Color";
  // clang-format on

 public:
  virtual ~Model();
  virtual void initVAO() = 0;
  virtual void paintGL(const glm::mat4 &mvpMat) = 0;
  virtual void tick(float time) = 0;
  void compileShaders();
  void addBackground(const std::shared_ptr<Background> &background) { _backgrounds->push_back(background); };
  void addObject(const t_object &object) { _objects->push_back(object); };
  void removeObject(const int index) {
    if (index >= 0 && index < getNumObjects()) {
      _objects->erase(_objects->begin() + index);
    }
  }
  t_objects getBackgrounds() { return _backgrounds; };
  GLuint getShaderID() { return _shaderID; };
  t_object getBackground(const int index) { return (*_backgrounds)[index]; };
  int getNumBackgrounds() { return (int)_backgrounds->size(); };
  t_objects getObjects() { return _objects; };
  t_object getObject(const int index) { return (*_objects)[index]; };
  int getNumObjects() { return (int)_objects->size(); };
  void setVertShaderPath(const t_string &vertShaderPath) { _vertShaderPath = vertShaderPath; };
  void setFragShaderPath(const t_string &fragShaderPath) { _fragShaderPath = fragShaderPath; };
  void setMaskMode(bool maskMode);
  void setRenderType(Primitives::RenderType renderType);
  void setBackgroundIDtoDraw(const int id) { _backgroundIDtoDraw = id; };
  void resetRenderType();
  void setBackgroundColor(const float r, const float g, const float b, const float a) {
    _backgroundColor[0] = r;
    _backgroundColor[1] = g;
    _backgroundColor[2] = b;
    _backgroundColor[3] = a;
  }
  void setBackgroundColor(std::array<float, 4> rgba) { _backgroundColor = rgba; }

  std::array<float, 4> getBackgroundColor() { return _backgroundColor; };
};
