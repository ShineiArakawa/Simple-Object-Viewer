#include <SimView/App/ViewerGUIApp.hpp>

#if defined(WIN32)
#include <Windows.h>
#endif  // WIN32

namespace simview {
namespace app {

using namespace window;
using namespace model;
using namespace util;

ViewerGUIApp::ViewerGUIApp()
    : _window(nullptr),
      _model(nullptr),
      _view(nullptr) {
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

  _window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

  if (_window == nullptr) {
    glfwTerminate();
    LOG_CRITICAL("Window creation failed!");
    exit(1);
  }

#ifdef WIN32
  {
    // Icon
    HWND hwnd = glfwGetWin32Window(_window);
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));

    if (hIcon) {
      SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
      SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    } else {
      LOG_WARN("Failed to load icon.");
    }
  }
#endif

  glfwMakeContextCurrent(_window);

  // NOTE: V-Sync
  // This option forces to use current monitor's FPS
  glfwSwapInterval(1);

  LOG_INFO(glfwGetVersionString());

  // ====================================================================
  // Initialize GLAD
  // ====================================================================
  if (gladLoadGL() == GL_FALSE) {
    LOG_CRITICAL("Failed to load OpenGL 3.x/4.x libraries by glad!");
    exit(1);
  }

  // ====================================================================
  // Initialize Model
  // ====================================================================
  _model = std::make_shared<ViewerModel>();

#ifdef DEBUG_SHADOW_MAPPING
  model->setModelVertShaderPath(std::make_shared<std::string>("C:/Users/araka/Projects/Simple-Object-Viewer/include/Shader/model.vert"));
  model->setModelFragShaderPath(std::make_shared<std::string>("C:/Users/araka/Projects/Simple-Object-Viewer/include/Shader/model.frag"));
  model->setDepthVertShaderPath(std::make_shared<std::string>("C:/Users/araka/Projects/Simple-Object-Viewer/include/Shader/depth.vert"));
  model->setDepthFragShaderPath(std::make_shared<std::string>("C:/Users/araka/Projects/Simple-Object-Viewer/include/Shader/depth.frag"));
#endif

  _model->compileShaders();

  // ====================================================================
  // Initialize UI
  // ====================================================================
  _view = std::make_shared<ImGuiMainView>(_window, _model);

#ifdef DEBUG_SHADOW_MAPPING
  auto box = std::make_shared<Box>(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
  box->setName("Box");
  _model->addObject(box);

  auto plane = std::make_shared<Box>(0.0f, -10.0f, 0.0f, 100.0f, 0.5f, 100.0f);
  plane->setName("Plane");
  _model->addObject(plane);

  _view->getSceneView()->isEnabledShadowMapping = true;
  _model->setIsEnabledShadowMapping(_view->getSceneView()->isEnabledShadowMapping);
#endif

#ifdef DEBUG_VTK
  auto vtkObj = std::make_shared<Object>("C:/Users/araka/Projects/Simple-Object-Viewer/data/cube.vtu");
  vtkObj->setName("VTK obj");
  _model->addObject(vtkObj);
#endif
}

ViewerGUIApp::~ViewerGUIApp() {
  glfwDestroyWindow(_window);
  glfwTerminate();
  LOG_INFO("Bye!");
}

void ViewerGUIApp::launch() {
  // ====================================================================
  // Start main loop
  // ====================================================================
  LOG_INFO("Start main loop.");

  while (!glfwWindowShouldClose(_window) && _view->toMoveOn()) {
    if (_view->shouldUpdate()) {  // NOTE: shouldUpdate() is needed for FPS control
      // Update view #########################
      _view->paint();
      // #####################################

      // Handle events #######################
      _view->listenEvent();
      // #####################################

      // Swap to another buffer ##############
      glfwSwapBuffers(_window);
      // #####################################
    }
  }

  LOG_INFO("Broke main loop.");
}

void ViewerGUIApp::addObject(const model::Primitive_t& object, bool toInitializeVAO) {
  _model->addObject(object, toInitializeVAO);
}

void ViewerGUIApp::addBackground(const model::Background_t& background) {
  _model->addBackground(background);
}

void ViewerGUIApp::setRenderType(const model::Primitive::RenderType renderType) {
  _view->setRenderType(renderType);
}

void ViewerGUIApp::setSideBarVisibility(const bool& isVisible) {
  _view->setSideBarVisibility(isVisible);
}

void ViewerGUIApp::setWindowSubTitle(const std::string& subTitle) {
  char title[2048]; // NOTE: Unsafe!
  sprintf_s(title, sizeof(title), "%s [%s]", WINDOW_TITLE, subTitle.c_str());

  glfwSetWindowTitle(_window, title);
}

}  // namespace app
}  // namespace simview
