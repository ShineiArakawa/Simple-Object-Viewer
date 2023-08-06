#include <App/main.hpp>

// ========================================================================
// Default Variables ======================================================
// ========================================================================
static int WIN_WIDTH = 1000;
static int WIN_HEIGHT = 1000;
static const double FPS = 60.0;
static const char* WIN_TITLE = "Renderer";
static const float CAMERA_MOVE_STEP = 1.0f;
static const float MODEL_ROTATE_STEP = 0.01f;
inline static const glm::vec3 DEFAULT_CAMERA_POS(20.0f, 0.0f, 0.0f);
inline static const glm::vec3 DEFAULT_CAMERA_LOOK_AT(0.0f, 0.0f, 0.0f);
inline static const glm::vec3 DEFAULT_CAMERA_UP(0.0f, 1.0f, 0.0f);
static const float DEFAULT_AC_SCALE = 1.0f;
static const float ROTATE_ANIMATION_ANGLE = glm::radians(1.0f);
enum ArcballMode { ARCBALL_MODE_NONE = 0x00, ARCBALL_MODE_TRANSLATE = 0x01, ARCBALL_MODE_ROTATE = 0x02, ARCBALL_MODE_SCALE = 0x04 };
// ========================================================================
// ========================================================================
// ========================================================================

// ========================================================================
// Global Variables =======================================================
// ========================================================================
typedef std::shared_ptr<Renderer> pRenderer;
typedef std::shared_ptr<Model> pModel;
pRenderer renderer = nullptr;
pModel model = nullptr;

bool isDragging = false;
bool isMaskMode = false;

bool enabledRotationgMode = false;

glm::ivec2 oldPos;
glm::ivec2 newPos;
glm::vec3 cameraPos = DEFAULT_CAMERA_POS;
glm::vec3 cameraLookAt = DEFAULT_CAMERA_LOOK_AT;
glm::vec3 cameraUp = DEFAULT_CAMERA_UP;

int arcballMode = ARCBALL_MODE_NONE;
float acScale = DEFAULT_AC_SCALE;
// ========================================================================
// ========================================================================
// ========================================================================

void resetCameraPose() {
  cameraPos = DEFAULT_CAMERA_POS;
  cameraLookAt = DEFAULT_CAMERA_LOOK_AT;
  cameraUp = DEFAULT_CAMERA_UP;
  acScale = DEFAULT_AC_SCALE;

  if (renderer != nullptr) {
    renderer->setViewMat(glm::lookAt(cameraPos, cameraLookAt, cameraUp));
    renderer->initMatrices();
  }
}

void resizeGL(GLFWwindow* window, int width, int height) {
  WIN_WIDTH = width;
  WIN_HEIGHT = height;

  glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

  int renderBufferWidth, renderBufferHeight;
  glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

  glViewport(0, 0, renderBufferWidth, renderBufferHeight);

  renderer->resizeGL();
}

void mouseEvent(GLFWwindow* window, int button, int action, int mods) {
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

// Convert screen-space coordinates to a position on the arcball sphere
glm::vec3 getVector(double x, double y) {
  // Assume a circle contacts internally with longer edges
  const int shortSide = std::min(WIN_WIDTH, WIN_HEIGHT);
  glm::vec3 pt(2.0f * x / (float)shortSide - 1.0f, -2.0f * y / (float)shortSide + 1.0f, 0.0f);

  // Calculate Z coordinate
  const double xySquared = pt.x * pt.x + pt.y * pt.y;
  if (xySquared <= 1.0) {
    // Calculate Z coordinate if a point is inside a unit circle
    pt.z = (float)std::sqrt(1.0 - xySquared);
  } else {
    // Suppose a point is on the circle line if the click position is outside
    // the unit circle
    pt = glm::normalize(pt);
  }

  return pt;
}

void updateRotate() {
  const glm::vec3 u = getVector(oldPos.x, oldPos.y);
  const glm::vec3 v = getVector(newPos.x, newPos.y);

  renderer->updateRotate(u, v);
}

void updateTranslate() {
  // NOTE:
  // This function assumes the object locates near to the world-space origin and
  // computes the amount of translation

  glm::vec4 originScreenSpace = renderer->getOriginScreenSpace();
  originScreenSpace /= originScreenSpace.w;

  // Calculate the start and end points of mouse motion, which depend Z
  // coordinate in screen space
  glm::vec4 newPosScreenSpace(2.0f * newPos.x / WIN_WIDTH - 1.0f, -2.0f * newPos.y / WIN_HEIGHT + 1.0f, originScreenSpace.z, 1.0f);
  glm::vec4 oldPosScreenSpace(2.0f * oldPos.x / WIN_WIDTH - 1.0f, -2.0f * oldPos.y / WIN_HEIGHT + 1.0f, originScreenSpace.z, 1.0f);

  renderer->updateTranslate(newPosScreenSpace, oldPosScreenSpace);
}

void updateScale() { renderer->updateScale(acScale); }

void updateTransform() {
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

void motionEvent(GLFWwindow* window, double xpos, double ypos) {
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

void wheelEvent(GLFWwindow* window, double xoffset, double yoffset) {
  acScale += (float)yoffset / 10.0f;
  if (acScale < 0) {
    acScale = 0.0f;
  }
  updateScale();
}

void keyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    auto cameraDirection = glm::normalize(cameraLookAt - cameraPos);

    if (key == GLFW_KEY_W) {
      cameraPos += cameraDirection * CAMERA_MOVE_STEP;
      renderer->setViewMat(glm::lookAt(cameraPos, cameraLookAt, cameraUp));
    } else if (key == GLFW_KEY_A) {
      glm::vec3 moveVec = glm::cross(cameraUp, cameraDirection) * CAMERA_MOVE_STEP;
      cameraPos += moveVec;
      cameraLookAt += moveVec;
      renderer->setViewMat(glm::lookAt(cameraPos, cameraLookAt, cameraUp));
    } else if (key == GLFW_KEY_S) {
      cameraPos += -cameraDirection * CAMERA_MOVE_STEP;
      renderer->setViewMat(glm::lookAt(cameraPos, cameraLookAt, cameraUp));
    } else if (key == GLFW_KEY_D) {
      glm::vec3 moveVec = glm::cross(cameraDirection, cameraUp) * CAMERA_MOVE_STEP;
      cameraPos += moveVec;
      cameraLookAt += moveVec;
      renderer->setViewMat(glm::lookAt(cameraPos, cameraLookAt, cameraUp));
    } else if (key == GLFW_KEY_M) {
      isMaskMode = !isMaskMode;
    } else if (key == GLFW_KEY_N) {
      model->setRenderType(Primitives::RenderType::NORMAL);
      isMaskMode = false;
    } else if (key == GLFW_KEY_C) {
      model->setRenderType(Primitives::RenderType::COLOR);
      isMaskMode = false;
    } else if (key == GLFW_KEY_T) {
      model->setRenderType(Primitives::RenderType::TEXTURE);
      isMaskMode = false;
    } else if (key == GLFW_KEY_V) {
      model->setRenderType(Primitives::RenderType::VERT_NORMAL);
      isMaskMode = false;
    } else if (key == GLFW_KEY_R) {
      model->resetRenderType();
      isMaskMode = false;
    } else if (key == GLFW_KEY_Q) {
      renderer->rotateModel(MODEL_ROTATE_STEP, cameraUp);
    } else if (key == GLFW_KEY_E) {
      renderer->rotateModel(-MODEL_ROTATE_STEP, cameraUp);
    } else if (key == GLFW_KEY_HOME) {
      resetCameraPose();
    } else if (key == GLFW_KEY_X) {
      enabledRotationgMode = !enabledRotationgMode;
    }

    model->setMaskMode(isMaskMode);
  }
}

int main(int argc, char** argv) {
  // Parse args
  int nArgs = argc - 1;
  std::cout << "Number of arguments : " + std::to_string(nArgs) << std::endl;
  for (int iArg = 0; iArg < argc; iArg++) {
    std::cout << "args[" << iArg << "]=" << argv[iArg] << std::endl;
  }

  std::string configFilePath;
  if (nArgs > 0) {
    configFilePath = argv[1];
  } else {
    std::cerr << "Invalid argument." << std::endl;
    std::cerr << "args: {config_file}" << std::endl;
    std::exit(1);
  }

  if (glfwInit() == GLFW_FALSE) {
    fprintf(stderr, "Initialization failed!\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a window
  GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL);
  if (window == NULL) {
    glfwTerminate();
    fprintf(stderr, "Window creation failed!\n");
    return 1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }
  std::cout << std::endl << "Load OpenGL " << glfwGetVersionString() << std::endl;

  model = std::make_shared<Model>();
  ModelParser::parse(configFilePath, model);
  model->setMaskMode(isMaskMode);

  renderer = std::make_shared<Renderer>(&WIN_WIDTH, &WIN_HEIGHT, model);
  resetCameraPose();

  glfwSetWindowSizeCallback(window, resizeGL);
  glfwSetMouseButtonCallback(window, mouseEvent);
  glfwSetCursorPosCallback(window, motionEvent);
  glfwSetScrollCallback(window, wheelEvent);
  glfwSetKeyCallback(window, keyboardEvent);

  {
    std::cout << std::endl << "### Start initilizing models ..." << std::endl;
    auto start = std::chrono::system_clock::now();
    renderer->initializeGL();
    auto end = std::chrono::system_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::cout << "### Finish initilizing models. Elapsed time is " << elapsedTime << " [sec]." << std::endl;
  }

  double prevTime = glfwGetTime();
  while (glfwWindowShouldClose(window) == GLFW_FALSE) {
    double currentTime = glfwGetTime();

    if (currentTime - prevTime >= 1.0 / FPS) {
      {
        double fps = 1.0 / (currentTime - prevTime);
        char winTitle[256];
        sprintf_s(winTitle, "%s (FPS: %.3f)", WIN_TITLE, fps);
        glfwSetWindowTitle(window, winTitle);
      }

      if (enabledRotationgMode) {
        renderer->rotateModel(ROTATE_ANIMATION_ANGLE, cameraUp);
      }

      renderer->paintGL();
      glfwSwapBuffers(window);
      glfwPollEvents();

      prevTime = currentTime;
    }
  }

  glfwDestroyWindow(window);
  glfwTerminate();
}