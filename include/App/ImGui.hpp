#pragma once
#ifndef __INCLUDE_GUI_HPP__

#include <OpenGL.hpp>

// clang-format off
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_glut.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <imgui_internal.h>
// clang-format on

#include <string>
#include <vector>

namespace gui {
class FrameBuffer {
 public:
  FrameBuffer(){};
  FrameBuffer(float width, float height);
  ~FrameBuffer();
  unsigned int getFrameTexture();
  void RescaleFrameBuffer(float width, float height);
  void Bind() const;
  void Unbind() const;
  float _width;
  float _height;

 private:
  GLuint _fbo;
  GLuint _texture;
  GLuint _rbo;
};

void loadTexture(const std::string &filePath, GLuint &texID);
}  // namespace gui

#define __INCLUDE_GUI_HPP__
#endif