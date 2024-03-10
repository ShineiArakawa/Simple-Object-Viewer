#include <App/PoneApp.hpp>

namespace simview {
namespace app {

using namespace window;
using namespace model;
using namespace renderer;
using namespace util;

void PoneApp::keyboardEventPone(GLFWwindow* window, int key, int scancode, int action, int mods) {
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
    } else if (key == GLFW_KEY_ENTER) {
      PoneModel::GamePhase currentPhase = model->getPhase();
      if (currentPhase == PoneModel::GamePhase::START) {
        model->setPhase(PoneModel::GamePhase::PLAYING);
      } else if (currentPhase == PoneModel::GamePhase::GAME_OVER) {
        model->setPhase(PoneModel::GamePhase::START);
        model->reset();
      }
    } else if (key == GLFW_KEY_LEFT) {
      model->paddleMoveLeft();
    } else if (key == GLFW_KEY_RIGHT) {
      model->paddleMoveRight();
    } else if (key == GLFW_KEY_ESCAPE) {
      model->setPhase(PoneModel::GamePhase::START);
      model->reset();
    }

    model->setMaskMode(Window::isMaskMode);
  }
}

PoneApp::PoneApp(const std::string& configFilePath) {
  if (glfwInit() == GLFW_FALSE) {
    fprintf(stderr, "Initialization failed!\n");
  }

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
    fprintf(stderr, "Window creation failed!\n");
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
  }
  std::cout << std::endl
            << "Load OpenGL " << glfwGetVersionString() << std::endl;

  model = std::make_shared<PoneModel>();
  ModelParser::parse(configFilePath, model);
  model->compileShaders();
  model->setMaskMode(Window::isMaskMode);

  Window::renderer = std::make_shared<Renderer>(&Window::WIN_WIDTH, &Window::WIN_HEIGHT, model);
  Window::resetCameraPose();

  glfwSetWindowSizeCallback(window, Window::resizeGL);
  glfwSetMouseButtonCallback(window, Window::mouseEvent);
  glfwSetCursorPosCallback(window, Window::motionEvent);
  glfwSetScrollCallback(window, Window::wheelEvent);
  glfwSetKeyCallback(window, keyboardEventPone);

  {
    std::cout << std::endl
              << "### Start initilizing models ..." << std::endl;
    auto start = std::chrono::system_clock::now();
    Window::renderer->initializeGL();
    auto end = std::chrono::system_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::cout << "### Finish initilizing models. Elapsed time is " << elapsedTime << " [sec]." << std::endl;
  }
}

PoneApp::~PoneApp() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void PoneApp::launch() {
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

}  // namespace app
}  // namespace simview