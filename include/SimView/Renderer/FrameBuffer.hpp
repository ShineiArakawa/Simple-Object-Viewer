#pragma once

#include <SimView/OpenGL.hpp>
#include <SimView/Util/Logging.hpp>

#define USE_MIP_MAP

namespace simview {
namespace renderer {

class FrameBuffer {
 public:
  FrameBuffer(){};
  FrameBuffer(const float width, const float height);
  ~FrameBuffer();

  unsigned int getFrameTexture();
  void rescaleFrameBuffer(const float width, const float height);
  void bind() const;
  void unbind() const;

  void setPixelValue(const int &x,
                     const int &y,
                     const uint8_t &&r,
                     const uint8_t &&g,
                     const uint8_t &&b,
                     const uint8_t &&a);

 private:
  GLuint _fbo;
  GLuint _texture;
  GLuint _rbo;
};

using FrameBuffer_t = std::shared_ptr<FrameBuffer>;

}  // namespace renderer
}  // namespace simview