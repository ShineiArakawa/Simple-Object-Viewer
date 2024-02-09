#include <Window/ImGuiWindow.hpp>

void ImGuiWindowHandler::createFrameBuffer() {
  // texture
  glGenFramebuffers(1, &frameBufferID);
  glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
  glCreateTextures(GL_TEXTURE_2D, 1, &textureBufferID);
  glBindTexture(GL_TEXTURE_2D, textureBufferID);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_WIDTH, WIN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBufferID, 0);

  glCreateTextures(GL_TEXTURE_2D, 1, &depthBufferID);
  glBindTexture(GL_TEXTURE_2D, depthBufferID);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, WIN_WIDTH, WIN_HEIGHT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthBufferID, 0);

  GLenum buffers[4] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(textureBufferID, buffers);

  unbindBuffer();
}

void ImGuiWindowHandler::resetCameraPose() {
  cameraPos = DEFAULT_CAMERA_POS;
  cameraLookAt = DEFAULT_CAMERA_LOOK_AT;
  cameraUp = DEFAULT_CAMERA_UP;
  acScale = DEFAULT_AC_SCALE;

  if (renderer != nullptr) {
    renderer->setViewMat(glm::lookAt(cameraPos, cameraLookAt, cameraUp));
    renderer->initMatrices();
  }
}

void ImGuiWindowHandler::resizeGL(int width, int height) {
  // WIN_WIDTH = width;
  // WIN_HEIGHT = height;

  // glBindTexture(GL_TEXTURE_2D, textureBufferID);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBufferID, 0);

  // glBindRenderbuffer(GL_RENDERBUFFER, renderBufferID);
  // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferID);

  // renderer->resizeGL();
}

void ImGuiWindowHandler::mouseEvent(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    arcballMode = ARCBALL_MODE_ROTATE;
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    arcballMode = ARCBALL_MODE_SCALE;
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    arcballMode = ARCBALL_MODE_TRANSLATE;
  }

  double px, py;
  glfwGetCursorPos(window, &px, &py);

  if (action == GLFW_PRESS) {
    if (!isDragging) {
      isDragging = true;
      oldPos = glm::ivec2(px, py);
      newPos = glm::ivec2(px, py);
    }
  } else {
    isDragging = false;
    oldPos = glm::ivec2(0, 0);
    newPos = glm::ivec2(0, 0);
    arcballMode = ARCBALL_MODE_NONE;
  }
}

glm::vec3 ImGuiWindowHandler::getVector(double x, double y) {
  const int shortSide = std::min(WIN_WIDTH, WIN_HEIGHT);
  glm::vec3 pt(2.0f * x / (float)shortSide - 1.0f, -2.0f * y / (float)shortSide + 1.0f, 0.0f);

  const double xySquared = pt.x * pt.x + pt.y * pt.y;
  if (xySquared <= 1.0) {
    pt.z = (float)std::sqrt(1.0 - xySquared);
  } else {
    pt = glm::normalize(pt);
  }

  return pt;
}

void ImGuiWindowHandler::updateRotate() {
  const glm::vec3 u = getVector(oldPos.x, oldPos.y);
  const glm::vec3 v = getVector(newPos.x, newPos.y);

  renderer->updateRotate(u, v);
}

void ImGuiWindowHandler::updateTranslate() {
  // NOTE:
  // This function assumes the object locates near to the world-space origin and
  // computes the amount of translation

  glm::vec4 originScreenSpace = renderer->getOriginScreenSpace();
  originScreenSpace /= originScreenSpace.w;

  glm::vec4 newPosScreenSpace(2.0f * newPos.x / WIN_WIDTH - 1.0f, -2.0f * newPos.y / WIN_HEIGHT + 1.0f, originScreenSpace.z, 1.0f);
  glm::vec4 oldPosScreenSpace(2.0f * oldPos.x / WIN_WIDTH - 1.0f, -2.0f * oldPos.y / WIN_HEIGHT + 1.0f, originScreenSpace.z, 1.0f);

  renderer->updateTranslate(newPosScreenSpace, oldPosScreenSpace);
}

void ImGuiWindowHandler::updateScale() { renderer->updateScale(acScale); }

void ImGuiWindowHandler::updateTransform() {
  switch (arcballMode) {
    case ARCBALL_MODE_ROTATE:
      updateRotate();
      break;

    case ARCBALL_MODE_TRANSLATE:
      updateTranslate();
      break;

    case ARCBALL_MODE_SCALE:
      acScale += (float)(oldPos.y - newPos.y) / WIN_HEIGHT;
      updateScale();
      break;
  }
}

void ImGuiWindowHandler::motionEvent(GLFWwindow* window, double xpos, double ypos) {
  if (isDragging) {
    newPos = glm::ivec2(xpos, ypos);

    const double dx = newPos.x - oldPos.x;
    const double dy = newPos.y - oldPos.y;
    const double length = dx * dx + dy * dy;
    if (length < 2.0 * 2.0) {
      return;
    } else {
      updateTransform();
      oldPos = glm::ivec2(xpos, ypos);
    }
  }
}

void ImGuiWindowHandler::wheelEvent(GLFWwindow* window, double xoffset, double yoffset) {
  acScale += (float)yoffset / 10.0f;
  if (acScale < 0) {
    acScale = 0.0f;
  }
  updateScale();
}

void ImGuiWindowHandler::bindBuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
  glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ImGuiWindowHandler::unbindBuffer() {
  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
