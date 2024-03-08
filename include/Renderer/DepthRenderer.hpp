#pragma once

#include <OpenGL.hpp>
#include <Shader/DefaultShaders.hpp>
#include <Shader/DepthShader.hpp>
#include <memory>

namespace simview {
namespace renderer {

class DepthRenderer {
 private:
  GLuint _depthMap;
  GLuint _depthMapFBO;
  shader::DepthShader_t _shader = nullptr;

 public:
  inline static const unsigned int DEPTH_MAP_WIDTH = 2048;
  inline static const unsigned int DEPTH_MAP_HEIGHT = 2048;

 private:
  // Nothing

 public:
  DepthRenderer(shader::DepthShader_t);
  ~DepthRenderer();

  void initDepthMap();

  GLuint getDepthMapId();

  void bind();
  void unbind();
};

using DepthRenderer_t = std::shared_ptr<DepthRenderer>;

}  // namespace renderer
}  // namespace simview