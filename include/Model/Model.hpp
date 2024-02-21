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
  template <class dtype>
  using t_vec4 = std::array<dtype, 4>;
  using t_object = std::shared_ptr<Primitives>;
  using t_background = std::shared_ptr<Background>;
  using t_objects = std::shared_ptr<std::vector<t_object>>;
  using t_string = std::shared_ptr<std::string>;

 private:
  // Nothing

 protected:
  t_objects _objects = std::make_shared<std::vector<t_object>>();
  t_objects _backgrounds = std::make_shared<std::vector<t_object>>();
  t_vec4<float> _backgroundColor = {0.0f, 0.0f, 0.0f, 1.0f};

  t_string _vertShaderPath = nullptr;
  t_string _fragShaderPath = nullptr;

  int _backgroundIDtoDraw = 0;
  float _time = 0.0f;
  GLuint _shaderID;

  float _lightPosition[3] = {5.0f, 0.0f, 0.0f};
  float _shininess = 50.0f;
  float _ambientIntensity = 0.1f;
  float _wireFrameColor[3] = {1.0f, 1.0f, 1.0f};
  float _wireFrameWidth = 0.01f;

 public:
  // clang-format off
  inline static const std::string KEY_MODEL = "Model";
  inline static const std::string KEY_BACKGROUND = "Background";
  inline static const std::string KEY_BACKGROUND_COLOR = "Color";
  // clang-format on

 public:
  virtual ~Model();
  virtual void initVAO() = 0;
  virtual void paintGL(const glm::mat4 &mvMat, const glm::mat4 &mvpMat, const glm::mat4 &normMat, const glm::mat4 &lightMat) = 0;
  virtual void tick(float time) = 0;

  void compileShaders();

  void addBackground(const t_background &background) { _backgrounds->push_back(background); };
  void addObject(const t_object &object) { _objects->push_back(object); };
  void removeObject(const int index) {
    if (index >= 0 && index < getNumObjects()) {
      _objects->erase(_objects->begin() + index);
    }
  }
  void removeBackground(const int index) {
    if (index >= 0 && index < getNumBackgrounds()) {
      _backgrounds->erase(_backgrounds->begin() + index);
    }
  }

  t_objects getBackgrounds() { return _backgrounds; };
  GLuint getShaderID() { return _shaderID; };
  t_object getBackground(const int index) { return (*_backgrounds)[index]; };
  int getNumBackgrounds() { return (int)_backgrounds->size(); };
  t_objects getObjects() { return _objects; };
  t_object getObject(const int index) { return (*_objects)[index]; };
  int getNumObjects() { return (int)_objects->size(); };
  int getBackgroundIDtoDraw() { return _backgroundIDtoDraw; };
  float *getPointerToLightPos() { return _lightPosition; };
  float *getPointerToShininess() { return &_shininess; };
  float *getPointerToAmbientIntensity() { return &_ambientIntensity; };
  int *getPointerToBackgroundIDtoDraw() { return &_backgroundIDtoDraw; };
  float *getPointerToWireFrameColor() { return _wireFrameColor; };
  float *getPointerToWireFrameWidth() { return &_wireFrameWidth; };

  void setVertShaderPath(const t_string &vertShaderPath) { _vertShaderPath = vertShaderPath; };
  void setFragShaderPath(const t_string &fragShaderPath) { _fragShaderPath = fragShaderPath; };

  void setMaskMode(bool maskMode);
  void setRenderType(Primitives::RenderType renderType);
  void setWireFrameMode(Primitives::WireFrameMode wireFrameMode);
  void setBackgroundIDtoDraw(const int index) { _backgroundIDtoDraw = index; };
  void resetRenderType();
  void setBackgroundColor(const float r, const float g, const float b, const float a) {
    _backgroundColor[0] = r;
    _backgroundColor[1] = g;
    _backgroundColor[2] = b;
    _backgroundColor[3] = a;
  }
  void setBackgroundColor(const t_vec4<float> &rgba) { _backgroundColor = rgba; }
  t_vec4<float> getBackgroundColor() { return _backgroundColor; };
};
