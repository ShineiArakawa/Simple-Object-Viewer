#pragma once

#include <ImGui.hpp>
#include <Model/Model.hpp>
#include <Renderer/Renderer.hpp>
#include <Util/StbAdapter.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace simview {
namespace window {
class ImGuiSceneView {
 public:
  using pRenderer = std::shared_ptr<renderer::Renderer>;
  using pModel = std::shared_ptr<model::Model>;

 private:
  pRenderer _renderer = nullptr;
  pModel _model = nullptr;
  GLFWwindow* _parentWindow = nullptr;

 public:
  // ========================================================================
  // Default Variables ======================================================
  // ========================================================================
  int WIN_WIDTH = 1000;
  int WIN_HEIGHT = 1000;
  const double FPS = 60.0;
  const float CAMERA_MOVE_STEP = 1.0f;
  const float MODEL_ROTATE_STEP = 0.01f;
  const glm::vec3 DEFAULT_CAMERA_POS = glm::vec3(30.0f, 0.0f, 0.0f);
  const glm::vec3 DEFAULT_CAMERA_LOOK_AT = glm::vec3(0.0f, 0.0f, 0.0f);
  const glm::vec3 DEFAULT_CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
  const float DEFAULT_AC_SCALE = 1.0f;
  enum ArcballMode { ARCBALL_MODE_NONE = 0x00,
                     ARCBALL_MODE_TRANSLATE = 0x01,
                     ARCBALL_MODE_ROTATE = 0x02,
                     ARCBALL_MODE_SCALE = 0x04 };
  // ========================================================================
  // ========================================================================
  // ========================================================================

  // ========================================================================
  // Global Variables =======================================================
  // ========================================================================

  bool isDragging = false;
  bool isMaskMode = false;
  bool isEnabledNormalMap = false;
  bool isEnabledShadowMapping = false;
  bool isVisibleBBOX = false;

  bool enabledModelRotationMode = false;
  bool enabledLightRotationMode = false;

  ImVec2 oldPos;
  ImVec2 newPos;
  glm::vec3 cameraPos = DEFAULT_CAMERA_POS;
  glm::vec3 cameraLookAt = DEFAULT_CAMERA_LOOK_AT;
  glm::vec3 cameraUp = DEFAULT_CAMERA_UP;

  int arcballMode = ARCBALL_MODE_NONE;
  float acScale = DEFAULT_AC_SCALE;
  float rotateAnimationAngle = 1.0f;
  // ========================================================================
  // ========================================================================
  // ========================================================================

  ImGuiSceneView(GLFWwindow* parentWindow, pModel model);
  ~ImGuiSceneView();

  renderer::Renderer::pFrameBuffer getFrameBuffer();
  pRenderer getRenderer();

  void paintGL();
  void resetCameraPose();
  void resetLightPose();
  void resizeGL(const int& width, const int& height);
  glm::vec3 getVector(const double& x, const double& y);

  void updateRotate();
  void updateTranslate();
  void updateScale();
  void updateTransform();

  void mouseEvent(const bool& isMouseOnScene, const ImVec2& relMousePos);
  void motionEvent(const bool& isMouseOnScene, const ImVec2& relMousePos);
  void wheelEvent(const bool& isMouseOnScene, const float& offset);

  void saveScreenShot(const std::string& filePath);
};
}  // namespace window
}  // namespace simview