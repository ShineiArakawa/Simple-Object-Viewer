#pragma once

#include <OpenGL.hpp>

class ShadowMapping {
 private:
  inline static const unsigned int DEPTH_MAP_WIDTH = 1024;
  inline static const unsigned int DEPTH_MAP_HEIGHT = 1024;

  GLuint _depthMap;
  GLuint _depthMapFBO;

 public:
  ShadowMapping();
  ~ShadowMapping();

  void initDepthMap();
  void bind();
  void unbind();
};