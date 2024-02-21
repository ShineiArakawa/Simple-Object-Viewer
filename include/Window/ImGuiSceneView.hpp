#pragma once
#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <ImGui.hpp>
#include <Model/Model.hpp>
#include <Renderer/Renderer.hpp>
#include <Util/StbAdapter.hpp>
#include <iostream>
#include <string>
#include <vector>

#define USE_MIP_MAP

class FrameBuffer {
 public:
  FrameBuffer(){};
  FrameBuffer(float width, float height);
  ~FrameBuffer();
  unsigned int getFrameTexture();
  void rescaleFrameBuffer(float width, float height);
  void bind() const;
  void unbind() const;

 private:
  GLuint _fbo;
  GLuint _texture;
  GLuint _rbo;
};

class ImGuiSceneView {
 public:
  typedef std::shared_ptr<Renderer> pRenderer;
  typedef std::shared_ptr<FrameBuffer> pFrameBuffer;

 private:
  inline static pRenderer _renderer = nullptr;
  inline static pFrameBuffer _frameBuffer = nullptr;
  inline static GLFWwindow* _parentWindow = nullptr;

 public:
  // ========================================================================
  // Default Variables ======================================================
  // ========================================================================
  inline static int WIN_WIDTH = 1000;
  inline static int WIN_HEIGHT = 1000;
  inline static const double FPS = 60.0;
  inline static const float CAMERA_MOVE_STEP = 1.0f;
  inline static const float MODEL_ROTATE_STEP = 0.01f;
  inline static const glm::vec3 DEFAULT_CAMERA_POS = glm::vec3(30.0f, 0.0f, 0.0f);
  inline static const glm::vec3 DEFAULT_CAMERA_LOOK_AT = glm::vec3(0.0f, 0.0f, 0.0f);
  inline static const glm::vec3 DEFAULT_CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
  inline static const float DEFAULT_AC_SCALE = 1.0f;
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

  inline static bool isDragging = false;
  inline static bool isMaskMode = false;

  inline static bool enabledModelRotationMode = false;
  inline static bool enabledLightRotationMode = false;

  inline static ImVec2 oldPos;
  inline static ImVec2 newPos;
  inline static glm::vec3 cameraPos = DEFAULT_CAMERA_POS;
  inline static glm::vec3 cameraLookAt = DEFAULT_CAMERA_LOOK_AT;
  inline static glm::vec3 cameraUp = DEFAULT_CAMERA_UP;

  inline static int arcballMode = ARCBALL_MODE_NONE;
  inline static float acScale = DEFAULT_AC_SCALE;
  inline static float rotateAnimationAngle = 1.0f;
  // ========================================================================
  // ========================================================================
  // ========================================================================

  ImGuiSceneView(GLFWwindow* parentWindow, pRenderer renderer);
  ~ImGuiSceneView();

  pFrameBuffer getFrameBuffer();

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

#endif