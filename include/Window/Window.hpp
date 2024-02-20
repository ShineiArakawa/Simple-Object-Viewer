#pragma once
#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <Model/Model.hpp>
#include <Renderer/Renderer.hpp>
#include <iostream>
#include <string>
#include <vector>

class Window {
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
  inline static const float ROTATE_ANIMATION_ANGLE = glm::radians(1.0f);
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
  typedef std::shared_ptr<Renderer> pRenderer;

  inline static pRenderer renderer = nullptr;

  inline static bool isDragging = false;
  inline static bool isMaskMode = false;

  inline static bool enabledModelRotationMode = false;

  inline static glm::ivec2 oldPos;
  inline static glm::ivec2 newPos;
  inline static glm::vec3 cameraPos = DEFAULT_CAMERA_POS;
  inline static glm::vec3 cameraLookAt = DEFAULT_CAMERA_LOOK_AT;
  inline static glm::vec3 cameraUp = DEFAULT_CAMERA_UP;

  inline static int arcballMode = ARCBALL_MODE_NONE;
  inline static float acScale = DEFAULT_AC_SCALE;
  // ========================================================================
  // ========================================================================
  // ========================================================================

  static void resetCameraPose();
  static void resizeGL(GLFWwindow* window, int width, int height);
  static glm::vec3 getVector(double x, double y);

  static void updateRotate();
  static void updateTranslate();
  static void updateScale();
  static void updateTransform();

  static void mouseEvent(GLFWwindow* window, int button, int action, int mods);
  static void motionEvent(GLFWwindow* window, double xpos, double ypos);
  static void wheelEvent(GLFWwindow* window, double xoffset, double yoffset);
};

#endif