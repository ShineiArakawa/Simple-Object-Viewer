#include <App/main.hpp>

typedef std::shared_ptr<Renderer> pRenderer;
typedef std::shared_ptr<Model> pModel;
pRenderer renderer = nullptr;
pModel model = nullptr;

static int WIN_WIDTH = 1000;
static int WIN_HEIGHT = 1000;
static const char* WIN_TITLE = "Renderer";
static const float CAMERA_MOVE_STEP = 1.0f;
static const float MODEL_ROTATE_STEP = 0.01f;

bool isDragging = false;
bool isMaskMode = false;

glm::ivec2 oldPos;
glm::ivec2 newPos;
glm::vec3 cameraPos(20.0f, 0.0f, 0.0f);
glm::vec3 cameraLookAt(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

enum ArcballMode {
  ARCBALL_MODE_NONE = 0x00,
  ARCBALL_MODE_TRANSLATE = 0x01,
  ARCBALL_MODE_ROTATE = 0x02,
  ARCBALL_MODE_SCALE = 0x04
};
int arcballMode = ARCBALL_MODE_NONE;
float acScale = 1.0f;

void resizeGL(GLFWwindow* window, int width, int height) {
  WIN_WIDTH = width;
  WIN_HEIGHT = height;

  glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

  int renderBufferWidth, renderBufferHeight;
  glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

  glViewport(0, 0, renderBufferWidth, renderBufferHeight);

  renderer->resizeGL();
}

// Callback for mouse click events
void mouseEvent(GLFWwindow* window, int button, int action, int mods) {
  // Switch following operation depending on a clicked button
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    arcballMode = ARCBALL_MODE_ROTATE;
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    arcballMode = ARCBALL_MODE_SCALE;
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    arcballMode = ARCBALL_MODE_TRANSLATE;
  }

  // Acquire a click position
  double px, py;
  glfwGetCursorPos(window, &px, &py);

  // Update state of mouse dragging
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
  glm::vec3 pt(2.0f * x / (float)shortSide - 1.0f,
               -2.0f * y / (float)shortSide + 1.0f, 0.0f);

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

// Update rotation matrix
void updateRotate() {
  const glm::vec3 u = getVector(oldPos.x, oldPos.y);
  const glm::vec3 v = getVector(newPos.x, newPos.y);

  renderer->updateRotate(u, v);
}

// Update translation matrix
void updateTranslate() {
  // NOTE:
  // This function assumes the object locates near to the world-space origin and
  // computes the amount of translation

  glm::vec4 originScreenSpace = renderer->getOriginScreenSpace();
  originScreenSpace /= originScreenSpace.w;

  // Calculate the start and end points of mouse motion, which depend Z
  // coordinate in screen space
  glm::vec4 newPosScreenSpace(2.0f * newPos.x / WIN_WIDTH - 1.0f,
                              -2.0f * newPos.y / WIN_HEIGHT + 1.0f,
                              originScreenSpace.z, 1.0f);
  glm::vec4 oldPosScreenSpace(2.0f * oldPos.x / WIN_WIDTH - 1.0f,
                              -2.0f * oldPos.y / WIN_HEIGHT + 1.0f,
                              originScreenSpace.z, 1.0f);

  renderer->updateTranslate(newPosScreenSpace, oldPosScreenSpace);
}

// Update object scale
void updateScale() { renderer->updateScale(acScale); }

// Update transformation matrices, depending on type of mouse interaction
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

// Callback for mouse move events
void motionEvent(GLFWwindow* window, double xpos, double ypos) {
  if (isDragging) {
    // Update current mouse position
    newPos = glm::ivec2(xpos, ypos);

    // Update transform only when mouse moves sufficiently
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

// Callback for mouse wheel event
void wheelEvent(GLFWwindow* window, double xoffset, double yoffset) {
  acScale += (float)yoffset / 10.0f;
  if (acScale < 0) {
    acScale = 0.0f;
  }
  updateScale();
}

void keyboardEvent(GLFWwindow* window, int key, int scancode, int action,
                   int mods) {
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    auto cameraDirection = glm::normalize(cameraLookAt - cameraPos);
    if (key == GLFW_KEY_W) {
      cameraPos += cameraDirection * CAMERA_MOVE_STEP;
    } else if (key == GLFW_KEY_A) {
      glm::vec3 moveVec =
          glm::cross(cameraUp, cameraDirection) * CAMERA_MOVE_STEP;
      cameraPos += moveVec;
      cameraLookAt += moveVec;
    } else if (key == GLFW_KEY_S) {
      cameraPos += -cameraDirection * CAMERA_MOVE_STEP;
    } else if (key == GLFW_KEY_D) {
      glm::vec3 moveVec =
          glm::cross(cameraDirection, cameraUp) * CAMERA_MOVE_STEP;
      cameraPos += moveVec;
      cameraLookAt += moveVec;
    }
    renderer->setViewMat(glm::lookAt(cameraPos, cameraLookAt, cameraUp));

    if (key == GLFW_KEY_M) {
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
    } else if (key == GLFW_KEY_R) {
      model->resetRenderType();
      isMaskMode = false;
    } else if (key == GLFW_KEY_Q) {
      renderer->rotateModel(MODEL_ROTATE_STEP, cameraUp);
    } else if (key == GLFW_KEY_E) {
      renderer->rotateModel(MODEL_ROTATE_STEP, cameraUp);
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

  // OpenGL initialization
  if (glfwInit() == GLFW_FALSE) {
    fprintf(stderr, "Initialization failed!\n");
    return 1;
  }

  // Specify OpenGL version (mandatory for Mac)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a window
  GLFWwindow* window =
      glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL);
  if (window == NULL) {
    glfwTerminate();
    fprintf(stderr, "Window creation failed!\n");
    return 1;
  }

  // Specify window as an OpenGL context
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // Check OpenGL version
  std::cout << "Load OpenGL " << glfwGetVersionString() << std::endl;

  // model = createModel();
  model = std::make_shared<Model>();
  ModelParser::parse(configFilePath, model);
  model->setMaskMode(isMaskMode);

  renderer = std::make_shared<Renderer>(&WIN_WIDTH, &WIN_HEIGHT, model);
  renderer->setViewMat(glm::lookAt(cameraPos, cameraLookAt, cameraUp));

  glfwSetWindowSizeCallback(window, resizeGL);
  glfwSetMouseButtonCallback(window, mouseEvent);
  glfwSetCursorPosCallback(window, motionEvent);
  glfwSetScrollCallback(window, wheelEvent);
  glfwSetKeyCallback(window, keyboardEvent);

  // User-specified initialization
  renderer->initializeGL();

  while (glfwWindowShouldClose(window) == GLFW_FALSE) {
    // Draw
    renderer->paintGL();

    // Swap drawing target buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Postprocess
  glfwDestroyWindow(window);
  glfwTerminate();
}