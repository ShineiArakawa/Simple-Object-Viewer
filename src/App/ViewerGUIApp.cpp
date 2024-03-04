#include <App/ViewerGUIApp.hpp>

ViewerGUIApp::ViewerGUIApp() : _window(nullptr),
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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  _window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

  if (_window == nullptr) {
    glfwTerminate();
    LOG_CRITICAL("Window creation failed!");
    exit(1);
  }

  glfwMakeContextCurrent(_window);
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
  // model->setModelVertShaderPath(std::make_shared<std::string>("C:/Users/PC_Arakawa/Projects/OpenGLTemplate/include/Shader/model.vert"));
  // model->setModelFragShaderPath(std::make_shared<std::string>("C:/Users/PC_Arakawa/Projects/OpenGLTemplate/include/Shader/model.frag"));
  // model->setDepthVertShaderPath(std::make_shared<std::string>("C:/Users/PC_Arakawa/Projects/OpenGLTemplate/include/Shader/depth.vert"));
  // model->setDepthFragShaderPath(std::make_shared<std::string>("C:/Users/PC_Arakawa/Projects/OpenGLTemplate/include/Shader/depth.frag"));
#endif

  _model->compileShaders();

  // ====================================================================
  // Initialize UI
  // ====================================================================
  _view = std::make_shared<ImGuiMainView>(_window, _model);

#ifdef DEBUG_SHADOW_MAPPING
  auto box = std::make_shared<Box>(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
  _model->addObject(box);

  auto plane = std::make_shared<Box>(0.0f, -10.0f, 0.0f, 100.0f, 0.5f, 100.0f);
  _model->addObject(plane);

  _model->setLightPosition(0.0f, 20.0f, 0.0f);

  _view->getSceneView()->isEnabledShadowMapping = true;
  _model->setIsEnabledShadowMapping(_view->getSceneView()->isEnabledShadowMapping);
#endif
}

ViewerGUIApp::~ViewerGUIApp() {
  glfwDestroyWindow(_window);
  glfwTerminate();
}

void ViewerGUIApp::launch() {
  // ====================================================================
  // Start main loop
  // ====================================================================
  while (!glfwWindowShouldClose(_window) && _view->moveOn) {
    _view->paint();
    _view->listenEvent();
    glfwSwapBuffers(_window);
  }
}

void ViewerGUIApp::addObject(const Model::t_object& object, bool toInitializeVAO) {
  _model->addObject(object, toInitializeVAO);
}

void ViewerGUIApp::addBackground(const Model::t_background& background) {
  _model->addBackground(background);
}