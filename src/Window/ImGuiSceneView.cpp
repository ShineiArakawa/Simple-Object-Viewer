#include <Window/ImGuiSceneView.hpp>

FrameBuffer::FrameBuffer(float width, float height) {
  glGenFramebuffers(1, &_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

  glGenTextures(1, &_texture);
  glBindTexture(GL_TEXTURE_2D, _texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

  glGenRenderbuffers(1, &_rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

FrameBuffer::~FrameBuffer() {}

unsigned int FrameBuffer::getFrameTexture() {
  return _texture;
}

void FrameBuffer::rescaleFrameBuffer(float width, float height) {
  glBindTexture(GL_TEXTURE_2D, _texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

  glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);
}

void FrameBuffer::bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

void FrameBuffer::unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ImGuiSceneView::ImGuiSceneView(GLFWwindow* parentWindow, pRenderer renderer) {
  _parentWindow = parentWindow;
  _frameBuffer = std::make_shared<FrameBuffer>(WIN_WIDTH, WIN_HEIGHT);
  _renderer = renderer;

  resetCameraPose();

  renderer->initializeGL();
}

ImGuiSceneView::~ImGuiSceneView() {}

ImGuiSceneView::pFrameBuffer ImGuiSceneView::getFrameBuffer() {
  return _frameBuffer;
}

void ImGuiSceneView::resetCameraPose() {
  cameraPos = DEFAULT_CAMERA_POS;
  cameraLookAt = DEFAULT_CAMERA_LOOK_AT;
  cameraUp = DEFAULT_CAMERA_UP;
  acScale = DEFAULT_AC_SCALE;

  if (_renderer != nullptr) {
    _renderer->setViewMat(glm::lookAt(cameraPos, cameraLookAt, cameraUp));
    _renderer->initMatrices();
  }
}

void ImGuiSceneView::paintGL() {
  _frameBuffer->bind();

  {
    if (enabledRotationgMode) {
      _renderer->rotateModel(ROTATE_ANIMATION_ANGLE, cameraUp);
    }
    _renderer->paintGL();
  }

  _frameBuffer->unbind();
}

void ImGuiSceneView::resizeGL(const int& width, const int& height) {
  WIN_WIDTH = width;
  WIN_HEIGHT = height;

  _frameBuffer->rescaleFrameBuffer(WIN_WIDTH, WIN_HEIGHT);

  glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

  _renderer->resizeGL();
}

glm::vec3 ImGuiSceneView::getVector(const double& x, const double& y) {
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

void ImGuiSceneView::updateRotate() {
  const glm::vec3 u = getVector(oldPos.x, oldPos.y);
  const glm::vec3 v = getVector(newPos.x, newPos.y);

  _renderer->updateRotate(u, v);
}

void ImGuiSceneView::updateTranslate() {
  // NOTE:
  // This function assumes the object locates near to the world-space origin and
  // computes the amount of translation

  glm::vec4 originScreenSpace = _renderer->getOriginScreenSpace();
  originScreenSpace /= originScreenSpace.w;

  glm::vec4 newPosScreenSpace(2.0f * newPos.x / WIN_WIDTH - 1.0f, -2.0f * newPos.y / WIN_HEIGHT + 1.0f, originScreenSpace.z, 1.0f);
  glm::vec4 oldPosScreenSpace(2.0f * oldPos.x / WIN_WIDTH - 1.0f, -2.0f * oldPos.y / WIN_HEIGHT + 1.0f, originScreenSpace.z, 1.0f);

  _renderer->updateTranslate(newPosScreenSpace, oldPosScreenSpace);
}

void ImGuiSceneView::updateScale() { _renderer->updateScale(acScale); }

void ImGuiSceneView::updateTransform() {
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

void ImGuiSceneView::mouseEvent(const bool& isMouseOnScene, const ImVec2& relMousePos) {
  if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    arcballMode = ARCBALL_MODE_ROTATE;
  } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
    arcballMode = ARCBALL_MODE_SCALE;
  } else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
    arcballMode = ARCBALL_MODE_TRANSLATE;
  }

  if (ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Middle) || ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
    if (!isDragging && isMouseOnScene) {
      isDragging = true;
      oldPos = relMousePos;
      newPos = relMousePos;
    }
  } else {
    isDragging = false;
    oldPos = ImVec2(0.0f, 0.0f);
    newPos = ImVec2(0.0f, 0.0f);
    arcballMode = ARCBALL_MODE_NONE;
  }
}

void ImGuiSceneView::motionEvent(const bool& isMouseOnScene, const ImVec2& relMousePos) {
  if (isDragging) {
    newPos = relMousePos;

    const float dx = newPos.x - oldPos.x;
    const float dy = newPos.y - oldPos.y;
    const float length = dx * dx + dy * dy;
    if (length < 2.0f * 2.0f) {
      return;
    } else {
      updateTransform();
      oldPos = relMousePos;
    }
  }
}

void ImGuiSceneView::wheelEvent(const bool& isMouseOnScene, const float& offset) {
  if (isMouseOnScene) {
    acScale += (float)offset / 10.0f;
    if (acScale < 0) {
      acScale = 0.0f;
    }
    updateScale();
  }
}
