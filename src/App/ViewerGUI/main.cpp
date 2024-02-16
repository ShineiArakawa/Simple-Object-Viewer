#include <App/ImGui.hpp>
#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <Util/FileUtil.hpp>
#include <Util/ModelParser.hpp>
#include <Window/Window.hpp>
#include <iostream>
#include <memory>

class SceneBuffer {
 public:
  inline static std::shared_ptr<gui::FrameBuffer> frameBuffer;
  inline static void setFrameBuffer(float width, float height) { SceneBuffer::frameBuffer = std::make_shared<gui::FrameBuffer>(width, height); };
  inline static void windowSizeCallback(GLFWwindow* window, int width, int height) {
    SceneBuffer::frameBuffer->rescaleFrameBuffer(width, height);
    glViewport(0, 0, width, height);
  };
};

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
    std::cout << "The config file path was not specified. Continue with the default config path: " << FileUtil::absPath("data/sample.json")
              << std::endl;
    configFilePath = "data/sample.json";
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

  GLFWwindow* window = glfwCreateWindow(Window::WIN_WIDTH, Window::WIN_HEIGHT, "test", NULL, NULL);

  if (window == NULL) {
    glfwTerminate();
    fprintf(stderr, "Window creation failed!\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  int bufferWidth, bufferHeight;
  glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);
  std::cout << "bufferWidth=" << bufferWidth << std::endl;
  std::cout << "bufferHeight=" << bufferHeight << std::endl;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (gladLoadGL() == 0) {
    fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
    return 1;
  }

  glViewport(0, 0, bufferWidth, bufferHeight);

  std::shared_ptr<ViewerModel> model = std::make_shared<ViewerModel>();
  ModelParser::parse(configFilePath, model);
  model->compileShaders();
  model->setMaskMode(Window::isMaskMode);

  Window::renderer = std::make_shared<Renderer>(&Window::WIN_WIDTH, &Window::WIN_HEIGHT, model);
  Window::resetCameraPose();
  SceneBuffer::setFrameBuffer(Window::WIN_WIDTH, Window::WIN_HEIGHT);
  Window::resizeGL(window, Window::WIN_WIDTH, Window::WIN_HEIGHT);
  Window::renderer->initializeGL();

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.IniFilename = nullptr;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  std::cout << "Start window loop !" << std::endl;
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::NewFrame();
    ImGui::Begin("My Scene");

    const float window_width = ImGui::GetContentRegionAvail().x;
    const float window_height = ImGui::GetContentRegionAvail().y;
    SceneBuffer::frameBuffer->rescaleFrameBuffer(window_width, window_height);
    Window::resizeGL(window, window_width, window_height);
    glViewport(0, 0, window_width, window_height);

    ImVec2 pos = ImGui::GetCursorScreenPos();

    ImGui::GetWindowDrawList()->AddImage(
        (void*)SceneBuffer::frameBuffer->getFrameTexture(),
        ImVec2(pos.x, pos.y),
        ImVec2(pos.x + window_width, pos.y + window_height),
        ImVec2(0, 1),
        ImVec2(1, 0));

    ImGui::End();
    ImGui::Render();

    // OpenGL Rendering
    SceneBuffer::frameBuffer->Bind();
    if (Window::enabledRotationgMode) {
      Window::renderer->rotateModel(Window::ROTATE_ANIMATION_ANGLE, Window::cameraUp);
    }
    Window::renderer->paintGL();
    SceneBuffer::frameBuffer->Unbind();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
