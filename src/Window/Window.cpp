#include <Window/Window.hpp>

void Window::resetCameraPose() {
  cameraPos = DEFAULT_CAMERA_POS;
  cameraLookAt = DEFAULT_CAMERA_LOOK_AT;
  cameraUp = DEFAULT_CAMERA_UP;
  acScale = DEFAULT_AC_SCALE;

  if (renderer != nullptr) {
    renderer->setViewMat(glm::lookAt(cameraPos, cameraLookAt, cameraUp));
    renderer->initMatrices();
  }
}

void Window::resizeGL(GLFWwindow* window, int width, int height) {
  WIN_WIDTH = width;
  WIN_HEIGHT = height;

  // glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

  int renderBufferWidth, renderBufferHeight;
  glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

  glViewport(0, 0, renderBufferWidth, renderBufferHeight);

  renderer->resizeGL();
}

void Window::mouseEvent(GLFWwindow* window, int button, int action, int mods) {
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

glm::vec3 Window::getVector(double x, double y) {
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

void Window::updateRotate() {
  const glm::vec3 u = getVector(oldPos.x, oldPos.y);
  const glm::vec3 v = getVector(newPos.x, newPos.y);

  renderer->updateRotate(u, v);
}

void Window::updateTranslate() {
  // NOTE:
  // This function assumes the object locates near to the world-space origin and
  // computes the amount of translation

  glm::vec4 originScreenSpace = renderer->getOriginScreenSpace();
  originScreenSpace /= originScreenSpace.w;

  glm::vec4 newPosScreenSpace(2.0f * newPos.x / WIN_WIDTH - 1.0f, -2.0f * newPos.y / WIN_HEIGHT + 1.0f, originScreenSpace.z, 1.0f);
  glm::vec4 oldPosScreenSpace(2.0f * oldPos.x / WIN_WIDTH - 1.0f, -2.0f * oldPos.y / WIN_HEIGHT + 1.0f, originScreenSpace.z, 1.0f);

  renderer->updateTranslate(newPosScreenSpace, oldPosScreenSpace);
}

void Window::updateScale() { renderer->updateScale(acScale); }

void Window::updateTransform() {
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

void Window::motionEvent(GLFWwindow* window, double xpos, double ypos) {
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

void Window::wheelEvent(GLFWwindow* window, double xoffset, double yoffset) {
  acScale += (float)yoffset / 10.0f;
  if (acScale < 0) {
    acScale = 0.0f;
  }
  updateScale();
}
