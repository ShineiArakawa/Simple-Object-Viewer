#include <SimView/Window/ImGuiSceneView.hpp>

namespace simview {
namespace window {

using namespace model;
using namespace renderer;
using namespace util;

ImGuiSceneView::ImGuiSceneView(GLFWwindow* parentWindow, Model_t model)
    : _parentWindow(parentWindow) {
  // ====================================================================
  // Initialize Renderer
  // ====================================================================
  _renderer = std::make_shared<Renderer>(&WIN_WIDTH,
                                         &WIN_HEIGHT,
                                         model,
                                         true);
  _renderer->initializeGL();

  resetCameraPose();
}

ImGuiSceneView::~ImGuiSceneView() {}

FrameBuffer_t ImGuiSceneView::getFrameBuffer() {
  return _renderer->getFrameBuffer();
}

Renderer_t ImGuiSceneView::getRenderer() {
  return _renderer;
}

void ImGuiSceneView::resetCameraPose() {
  cameraPos = DEFAULT_CAMERA_POS;
  cameraLookAt = DEFAULT_CAMERA_LOOK_AT;
  cameraUp = DEFAULT_CAMERA_UP;
  acScale = DEFAULT_AC_SCALE;

  if (_renderer != nullptr) {
    _renderer->setViewMat(glm::lookAt(cameraPos, cameraLookAt, cameraUp));
    _renderer->initModelMatrices();
  }
}

void ImGuiSceneView::resetLightPose() {
  if (_renderer != nullptr) {
    _renderer->initLightMatrices();
  }
}

void ImGuiSceneView::paintGL() {
  // =======================================================================================
  // Update model rotation animation
  // =======================================================================================
  if (enabledModelRotationMode) {
    _renderer->rotateModel(glm::radians(rotateAnimationAngle), cameraUp);
  }

  // =======================================================================================
  // Update light rotation animation
  // =======================================================================================
  if (enabledLightRotationMode) {
    _renderer->rotateLight(glm::radians(rotateAnimationAngle), cameraUp);
  }

  // =======================================================================================
  // Draw scene
  // =======================================================================================
  _renderer->paintGL(isEnabledShadowMapping);
}

void ImGuiSceneView::resizeGL(const int& width, const int& height) {
  WIN_WIDTH = width;
  WIN_HEIGHT = height;

  glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

  _renderer->resizeGL();
}

glm::vec3 ImGuiSceneView::getVector(const double& x, const double& y) {
  // const int shortSide = std::min(WIN_WIDTH, WIN_HEIGHT);
  glm::vec3 pt(2.0f * x / (float)WIN_WIDTH - 1.0f,
               -2.0f * y / (float)WIN_HEIGHT + 1.0f,
               0.0f);

  const double xySquared = pt.x * pt.x + pt.y * pt.y;
  if (xySquared <= 1.0) {
    pt.z = (float)std::sqrt(1.0 - xySquared);
  } else {
    pt = glm::normalize(pt);
  }

  return pt;
}

void ImGuiSceneView::updateRotate() {
  const glm::vec3& u = getVector(oldPos.x, oldPos.y);
  const glm::vec3& v = getVector(newPos.x, newPos.y);

  _renderer->updateRotate(u, v);
}

void ImGuiSceneView::updateTranslate() {
  // NOTE:
  // This function assumes the object locates near to the world-space origin and
  // computes the amount of translation

  glm::vec4 originScreenSpace = _renderer->getOriginScreenSpace();
  originScreenSpace /= originScreenSpace.w;

  glm::vec4 newPosScreenSpace(2.0f * newPos.x / WIN_WIDTH - 1.0f,
                              -2.0f * newPos.y / WIN_HEIGHT + 1.0f,
                              originScreenSpace.z,
                              1.0f);
  glm::vec4 oldPosScreenSpace(2.0f * oldPos.x / WIN_WIDTH - 1.0f,
                              -2.0f * oldPos.y / WIN_HEIGHT + 1.0f,
                              originScreenSpace.z,
                              1.0f);

  _renderer->updateTranslate(newPosScreenSpace, oldPosScreenSpace);
}

void ImGuiSceneView::updateScale() {
  _renderer->updateScale(acScale);
}

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

    if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
      // Pressed 'l-Shift' key
      if (relMousePos.x >= 0.0f && relMousePos.x < WIN_WIDTH && relMousePos.y >= 0.0f && relMousePos.y < WIN_HEIGHT) {
        getFrameBuffer()->setPixelValue((int)relMousePos.x, (int)relMousePos.y, 255, 255, 255, 255);
      }
    } else {
      if (length < 2.0f * 2.0f) {
        return;
      } else {
        updateTransform();
        oldPos = relMousePos;
      }
    }
  }
}

void ImGuiSceneView::wheelEvent(const bool& isMouseOnScene, const float& offset) {
  if (isMouseOnScene) {
    acScale += (float)offset / 10.0f;

    if (acScale < 0.0f) {
      acScale = 0.0001f;
    }

    updateScale();
  }
}

void ImGuiSceneView::saveScreenShot(const std::string& filePath) {
  const auto frameBuffer = _renderer->getFrameBuffer();

  if (frameBuffer != nullptr) {
    unsigned char* bytesTexture = (unsigned char*)malloc(sizeof(unsigned char) * WIN_WIDTH * WIN_HEIGHT * 4);

    glBindTexture(GL_TEXTURE_2D, frameBuffer->getFrameTexture());
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytesTexture);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Transpose
    unsigned char* bytesTransposedTexture = (unsigned char*)malloc(sizeof(unsigned char) * WIN_WIDTH * WIN_HEIGHT * 4);
    for (int w = 0; w < WIN_WIDTH; ++w) {
      for (int h = 0; h < WIN_HEIGHT; ++h) {
        const int distIndex = 4 * (h * WIN_WIDTH + w);
        const int srcIndex = 4 * ((WIN_HEIGHT - h - 1) * WIN_WIDTH + w);

        bytesTransposedTexture[distIndex + 0] = bytesTexture[srcIndex + 0];
        bytesTransposedTexture[distIndex + 1] = bytesTexture[srcIndex + 1];
        bytesTransposedTexture[distIndex + 2] = bytesTexture[srcIndex + 2];
        bytesTransposedTexture[distIndex + 3] = bytesTexture[srcIndex + 3];
      }
    }

    stb::saveImage(WIN_WIDTH, WIN_HEIGHT, 4, bytesTransposedTexture, filePath);

    free(bytesTexture);
    free(bytesTransposedTexture);
  } else {
    LOG_WARN("The scene was not rendered to the current frame buffer.");
  }
}

}  // namespace window
}  // namespace simview