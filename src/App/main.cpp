#include <App/main.hpp>

typedef std::shared_ptr<Renderer> pRenderer;
typedef std::shared_ptr<Model> pModel;
pRenderer renderer = nullptr;
pModel model = nullptr;

static int WIN_WIDTH = 1000;
static int WIN_HEIGHT = 1000;
static const char* WIN_TITLE = "Renderer";
static const float CAMERA_MOVE_STEP = 0.01f;

bool isDragging = false;
bool isMaskMode = false;

glm::ivec2 oldPos;
glm::ivec2 newPos;
glm::vec3 cameraPos(20.0f, 0.0f, 0.0f);
glm::vec3 cameraLookAt(0.0f, 0.0f, 0.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

enum ArcballMode { ARCBALL_MODE_NONE = 0x00, ARCBALL_MODE_TRANSLATE = 0x01, ARCBALL_MODE_ROTATE = 0x02, ARCBALL_MODE_SCALE = 0x04 };
int arcballMode = ARCBALL_MODE_NONE;
float acScale = 1.0f;

void resizeGL(GLFWwindow* window, int width, int height) {
  // Update user-managed window size
  WIN_WIDTH = width;
  WIN_HEIGHT = height;

  // Update GLFW-managed window size
  glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

  // Get actual window size by pixels
  int renderBufferWidth, renderBufferHeight;
  glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

  // Update viewport transform
  glViewport(0, 0, renderBufferWidth, renderBufferHeight);

  // Update projection matrix
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
  glm::vec3 pt(2.0f * x / (float)shortSide - 1.0f, -2.0f * y / (float)shortSide + 1.0f, 0.0f);

  // Calculate Z coordinate
  const double xySquared = pt.x * pt.x + pt.y * pt.y;
  if (xySquared <= 1.0) {
    // Calculate Z coordinate if a point is inside a unit circle
    pt.z = std::sqrt(1.0 - xySquared);
  } else {
    // Suppose a point is on the circle line if the click position is outside the unit circle
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
  // This function assumes the object locates near to the world-space origin and computes the amount of translation

  glm::vec4 originScreenSpace = renderer->getOriginScreenSpace();
  originScreenSpace /= originScreenSpace.w;

  // Calculate the start and end points of mouse motion, which depend Z coordinate in screen space
  glm::vec4 newPosScreenSpace(2.0f * newPos.x / WIN_WIDTH - 1.0f, -2.0f * newPos.y / WIN_HEIGHT + 1.0f, originScreenSpace.z, 1.0f);
  glm::vec4 oldPosScreenSpace(2.0f * oldPos.x / WIN_WIDTH - 1.0f, -2.0f * oldPos.y / WIN_HEIGHT + 1.0f, originScreenSpace.z, 1.0f);

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
  acScale += yoffset / 10.0;
  if (acScale < 0) {
    acScale = 0.0f;
  }
  updateScale();
}

void keyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    printf("Keyboard: Press\n");
  } else if (action == GLFW_RELEASE) {
    printf("Keyboard: Release\n");
  } else if (action == GLFW_REPEAT) {
    printf("Keyboard: Repeat\n");
  } else {
    printf("Unknown press/release event!!\n");
  }

  if (key >= 0 && key < 127) {
    // ASCII文字は127未満の整数
    printf("Key: %c (%d)\n", (char)key, key);
  }

  // 特殊キーが押されているかの判定
  int specialKeys[] = {GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER};
  const char* specialKeyNames[] = {"Shift", "Ctrl", "Alt", "Super"};

  if (action == GLFW_RELEASE) {
    if (key == GLFW_KEY_W) {
      cameraPos += glm::normalize(cameraLookAt) * CAMERA_MOVE_STEP;
    } else if (key == GLFW_KEY_A) {
      cameraPos += glm::normalize(cameraLookAt) * CAMERA_MOVE_STEP;
    } else if (key == GLFW_KEY_S) {
      cameraPos += -glm::normalize(cameraLookAt) * CAMERA_MOVE_STEP;
    } else if (key == GLFW_KEY_D) {
      cameraPos += glm::normalize(cameraLookAt) * CAMERA_MOVE_STEP;
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
    }

    model->setMaskMode(isMaskMode);
  }
}

std::shared_ptr<Model> createModel() {
  pModel model = std::make_shared<Model>();
  GLuint shaderID = ShaderCompiler::buildShaderProgram(ShaderCompiler::VERT_SHADER_FILE, ShaderCompiler::FRAG_SHADER_FILE);

  {
      // int nBoxesX = 1;
      // int nBoxesY = 1;
      // int nBoxesZ = 1;
      // float interval = 3.0f;
      // float minX = -((float)nBoxesX * interval) / 2.0f;
      // float minY = -((float)nBoxesY * interval) / 2.0f;
      // float minZ = -((float)nBoxesZ * interval) / 2.0f;

      // for (int x = 0; x < nBoxesX; x++) {
      //   for (int y = 0; y < nBoxesY; y++) {
      //     for (int z = 0; z < nBoxesZ; z++) {
      //       float offsetX = minX + (float)x * interval;
      //       float offsetY = minY + (float)y * interval;
      //       float offsetZ = minZ + (float)z * interval;
      //       // std::shared_ptr<Primitives> box = std::make_shared<Box>(offsetX, offsetY, offsetZ);
      //       // box->setShader(shaderID);
      //       // model->addObject(box);

      //       std::string filePath = "C:/Users/araka/Projects/OpenGLTemplate/data/bunny.obj";
      //       std::shared_ptr<Primitives> bunny = std::make_shared<Object>(filePath, offsetX, offsetY, offsetZ);
      //       bunny->setShader(shaderID);
      //       bunny->setDefaultRenderType(Primitives::RenderType::COLOR);
      //       model->addObject(std::move(bunny));
      //     }
      //   }
      // }
  }

  {
      // std::shared_ptr<Primitives> box = std::make_shared<Box>(5.0f, 0.0f, 0.0f);
      // box->setShader(shaderID);
      // box->setDefaultRenderType(Primitives::RenderType::COLOR);
      // model->addObject(box);
  }

  {
    // std::string filePathBackground = "../../../data/m0000002_H_12_Diffuse.png";
    // std::shared_ptr<Background> background = std::make_shared<Background>(filePathBackground);
    // background->setShader(shaderID);
    // model->setBackground(background);
  }

  {
      // std::string filePathTerrain = "../../../data/terrain.png";
      // std::shared_ptr<Primitives> terrain = std::make_shared<Terrain>(filePathTerrain, 0.0f, 0.0f, 0.0f);
      // terrain->setShader(shaderID);
      // terrain->setDefaultRenderType(Primitives::RenderType::NORMAL);
      // model->addObject(std::move(terrain));
  }

  {
    std::string filePathFace = "../../../data/m0000002_cleanup.obj";
    std::string filePathFaceTexture = "../../../data/m0000002_H_12_Diffuse.png";
    std::shared_ptr<Object> face = std::make_shared<Object>(filePathFace, 0.0f, 0.0f, 0.0f, 0.01f);
    face->setShader(shaderID);
    face->loadTexture(filePathFaceTexture);
    face->setDefaultRenderType(Primitives::RenderType::NORMAL);
    model->addObject(std::move(face));
  }

  return model;
}

int main(int argc, char** argv) {
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
  GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL);
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

  model = createModel();
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