#pragma once

#include <OpenGL.hpp>
#include <Shader/DefaultShaders.hpp>
#include <Shader/DepthShader.hpp>
#include <memory>

class DepthRenderer {
 public:
  using DepthShader_t = std::shared_ptr<DepthShader>;

 private:
  GLuint _depthMap;
  GLuint _depthMapFBO;
  DepthShader_t _shader = nullptr;

 public:
  inline static const unsigned int DEPTH_MAP_WIDTH = 1024;
  inline static const unsigned int DEPTH_MAP_HEIGHT = 1024;

 private:
  // Nothing

 public:
  DepthRenderer(DepthShader_t);
  ~DepthRenderer();

  void initDepthMap();

  GLuint getDepthMapId();

  void bind();
  void unbind();
};