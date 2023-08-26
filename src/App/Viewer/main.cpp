#include <App/main.hpp>

static pViewerModel model = nullptr;
static const char* WIN_TITLE = "Viewer";

void keyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    auto cameraDirection = glm::normalize(Window::cameraLookAt - Window::cameraPos);

    if (key == GLFW_KEY_W) {
      Window::cameraPos += cameraDirection * Window::CAMERA_MOVE_STEP;
      Window::renderer->setViewMat(glm::lookAt(Window::cameraPos, Window::cameraLookAt, Window::cameraUp));
    } else if (key == GLFW_KEY_A) {
      glm::vec3 moveVec = glm::cross(Window::cameraUp, cameraDirection) * Window::CAMERA_MOVE_STEP;
      Window::cameraPos += moveVec;
      Window::cameraLookAt += moveVec;
      Window::renderer->setViewMat(glm::lookAt(Window::cameraPos, Window::cameraLookAt, Window::cameraUp));
    } else if (key == GLFW_KEY_S) {
      Window::cameraPos += -cameraDirection * Window::CAMERA_MOVE_STEP;
      Window::renderer->setViewMat(glm::lookAt(Window::cameraPos, Window::cameraLookAt, Window::cameraUp));
    } else if (key == GLFW_KEY_D) {
      glm::vec3 moveVec = glm::cross(cameraDirection, Window::cameraUp) * Window::CAMERA_MOVE_STEP;
      Window::cameraPos += moveVec;
      Window::cameraLookAt += moveVec;
      Window::renderer->setViewMat(glm::lookAt(Window::cameraPos, Window::cameraLookAt, Window::cameraUp));
    } else if (key == GLFW_KEY_M) {
      Window::isMaskMode = !Window::isMaskMode;
    } else if (key == GLFW_KEY_N) {
      model->setRenderType(Primitives::RenderType::NORMAL);
      Window::isMaskMode = false;
    } else if (key == GLFW_KEY_C) {
      model->setRenderType(Primitives::RenderType::COLOR);
      Window::isMaskMode = false;
    } else if (key == GLFW_KEY_T) {
      model->setRenderType(Primitives::RenderType::TEXTURE);
      Window::isMaskMode = false;
    } else if (key == GLFW_KEY_V) {
      model->setRenderType(Primitives::RenderType::VERT_NORMAL);
      Window::isMaskMode = false;
    } else if (key == GLFW_KEY_R) {
      model->resetRenderType();
      Window::isMaskMode = false;
    } else if (key == GLFW_KEY_Q) {
      Window::renderer->rotateModel(Window::MODEL_ROTATE_STEP, Window::cameraUp);
    } else if (key == GLFW_KEY_E) {
      Window::renderer->rotateModel(-Window::MODEL_ROTATE_STEP, Window::cameraUp);
    } else if (key == GLFW_KEY_HOME) {
      Window::resetCameraPose();
    } else if (key == GLFW_KEY_X) {
      Window::enabledRotationgMode = !Window::enabledRotationgMode;
    }

    model->setMaskMode(Window::isMaskMode);
  }
}

int main(int argc, char** argv) {
  // Parse args
  int nArgs = argc - 1;
  std::cout << "Number of arguments : " + std::to_string(nArgs) << std::endl;
  for (int iArg = 0; iArg < nArgs; iArg++) {
    std::cout << "args[" << iArg << "]=" << argv[iArg + 1] << std::endl;
  }

  // Parse config file path
  std::string configFilePath;
  if (nArgs > 0) {
    configFilePath = argv[1];
  } else {
    std::cout << "The config file path was not specified. Continue with the default config path: " << FileUtil::absPath(DEFAULT_CONFIG_PATH)
              << std::endl;
    configFilePath = DEFAULT_CONFIG_PATH;
  }

  // Check config file
  if (!FileUtil::exists(configFilePath)) {
    std::cerr << "Failed to open the config file. Please check the arguments." << std::endl;
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
  GLFWwindow* window = glfwCreateWindow(Window::WIN_WIDTH, Window::WIN_HEIGHT, WIN_TITLE, NULL, NULL);
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

  model = std::make_shared<ViewerModel>();
  ModelParser::parse(configFilePath, model);
  model->setMaskMode(Window::isMaskMode);

  Window::renderer = std::make_shared<Renderer>(&Window::WIN_WIDTH, &Window::WIN_HEIGHT, model);
  Window::resetCameraPose();

  glfwSetWindowSizeCallback(window, Window::resizeGL);
  glfwSetMouseButtonCallback(window, Window::mouseEvent);
  glfwSetCursorPosCallback(window, Window::motionEvent);
  glfwSetScrollCallback(window, Window::wheelEvent);
  glfwSetKeyCallback(window, keyboardEvent);

  {
    std::cout << std::endl << "### Start initilizing models ..." << std::endl;
    auto start = std::chrono::system_clock::now();
    Window::renderer->initializeGL();
    auto end = std::chrono::system_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::cout << "### Finish initilizing models. Elapsed time is " << elapsedTime << " [sec]." << std::endl;
  }

  double prevTime = glfwGetTime();
  while (glfwWindowShouldClose(window) == GLFW_FALSE) {
    double currentTime = glfwGetTime();

    if (currentTime - prevTime >= 1.0 / Window::FPS) {
      {
        double fps = 1.0 / (currentTime - prevTime);
        char winTitle[256];
        sprintf(winTitle, "%s (FPS: %.3f)", WIN_TITLE, fps);
        glfwSetWindowTitle(window, winTitle);
      }

      if (Window::enabledRotationgMode) {
        Window::renderer->rotateModel(Window::ROTATE_ANIMATION_ANGLE, Window::cameraUp);
      }

      Window::renderer->paintGL();
      glfwSwapBuffers(window);
      glfwPollEvents();

      prevTime = currentTime;
    }
  }

  glfwDestroyWindow(window);
  glfwTerminate();
}
