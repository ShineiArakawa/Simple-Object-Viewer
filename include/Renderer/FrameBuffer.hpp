#pragma once

#include <OpenGL.hpp>
#include <Util/Logging.hpp>

#define USE_MIP_MAP

class FrameBuffer {
 public:
  FrameBuffer(){};
  FrameBuffer(const float width, const float height);
  ~FrameBuffer();
  unsigned int getFrameTexture();
  void rescaleFrameBuffer(const float width, const float height);
  void bind() const;
  void unbind() const;

 private:
  GLuint _fbo;
  GLuint _texture;
  GLuint _rbo;
};