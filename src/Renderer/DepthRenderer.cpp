#include <Renderer/DepthRenderer.hpp>

namespace simview {
namespace renderer {

using namespace shader;

DepthRenderer::DepthRenderer(DepthShader_t shader) : _shader(shader) {
  initDepthMap();
}

DepthRenderer::~DepthRenderer() {
}

void DepthRenderer::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
}

void DepthRenderer::unbind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthRenderer::initDepthMap() {
  glGenFramebuffers(1, &_depthMapFBO);

  glGenTextures(1, &_depthMap);
  glBindTexture(GL_TEXTURE_2D, _depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, DEPTH_MAP_WIDTH, DEPTH_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  float borderColor[] = {1.0, 1.0, 1.0, 1.0};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);

  glDrawBuffer(GL_NONE);
  // glReadBuffer(GL_NONE);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    LOG_ERROR("Depth frame buffer is incomplete!");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint DepthRenderer::getDepthMapId() {
  return _depthMap;
}

}  // namespace renderer
}  // namespace simview