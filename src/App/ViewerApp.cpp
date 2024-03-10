#include <App/ViewerApp.hpp>

namespace simview {
namespace app {

using namespace window;
using namespace model;
using namespace renderer;
using namespace util;

ViewerApp::ViewerApp(std::string configFilePath) {
  Logging::setLevelFromEnv();

  // ====================================================================
  // Initialize GLFW window
  // ====================================================================
  if (glfwInit() == GLFW_FALSE) {
    LOG_CRITICAL("GLFW initialization failed!");
    exit(1);
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a window
  window = glfwCreateWindow(Window::WIN_WIDTH,
                            Window::WIN_HEIGHT,
                            WIN_TITLE,
                            NULL,
                            NULL);

  if (window == NULL) {
    glfwTerminate();
    LOG_CRITICAL("Window creation failed!");
    exit(1);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  LOG_INFO(glfwGetVersionString());

  // ====================================================================
  // Initialize GLAD
  // ====================================================================
  if (gladLoadGL() == GL_FALSE) {
    LOG_CRITICAL("Failed to load OpenGL 3.x/4.x libraries by glad!");
    exit(1);
  }

  model = std::make_shared<ViewerModel>();
  model->compileShaders();
  ModelParser::parse(configFilePath, model);
  {
    LOG_INFO("### Start initilizing models ...");
    const auto start = std::chrono::system_clock::now();
    model->initVAO();
    const auto end = std::chrono::system_clock::now();
    const double elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    LOG_INFO("### Finish initilizing models. Elapsed time is " + std::to_string(elapsedTime) + " [sec].");
  }
  model->setMaskMode(Window::isMaskMode);

  Window::renderer = std::make_shared<Renderer>(&Window::WIN_WIDTH, &Window::WIN_HEIGHT, model);
  Window::renderer->initializeGL();
  Window::resetCameraPose();

  glfwSetWindowSizeCallback(window, Window::resizeGL);
  glfwSetMouseButtonCallback(window, Window::mouseEvent);
  glfwSetCursorPosCallback(window, Window::motionEvent);
  glfwSetScrollCallback(window, Window::wheelEvent);
  glfwSetKeyCallback(window, keyboardEventViewer);
}

ViewerApp::~ViewerApp() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void ViewerApp::launch() {
  double prevTime = glfwGetTime();
  while (glfwWindowShouldClose(window) == GLFW_FALSE) {
    const double currentTime = glfwGetTime();

    if (currentTime - prevTime >= 1.0 / Window::FPS) {
      {
        const double fps = 1.0 / (currentTime - prevTime);
        char winTitle[256];
        sprintf(winTitle, "%s (FPS: %.3f)", WIN_TITLE, fps);
        glfwSetWindowTitle(window, winTitle);
      }

      if (Window::enabledModelRotationMode) {
        Window::renderer->rotateModel(Window::ROTATE_ANIMATION_ANGLE, Window::cameraUp);
      }

      Window::renderer->paintGL();
      glfwSwapBuffers(window);
      glfwPollEvents();

      prevTime = currentTime;
    }
  }
}

void ViewerApp::keyboardEventViewer(GLFWwindow* window, int key, int scancode, int action, int mods) {
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
      model->setRenderType(Primitive::RenderType::NORMAL);
      Window::isMaskMode = false;
    } else if (key == GLFW_KEY_C) {
      model->setRenderType(Primitive::RenderType::COLOR);
      Window::isMaskMode = false;
    } else if (key == GLFW_KEY_T) {
      model->setRenderType(Primitive::RenderType::TEXTURE);
      Window::isMaskMode = false;
    } else if (key == GLFW_KEY_V) {
      model->setRenderType(Primitive::RenderType::VERT_NORMAL);
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
      Window::enabledModelRotationMode = !Window::enabledModelRotationMode;
    }

    model->setMaskMode(Window::isMaskMode);
  }
}

}  // namespace app
}  // namespace simview