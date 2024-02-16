#include <App/ImGui.hpp>
#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <Util/FileUtil.hpp>
#include <Util/ModelParser.hpp>
#include <Window/Window.hpp>
#include <iostream>
#include <memory>

GLuint indexBufferId;
GLuint vertexBufferId;

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

  GLFWwindow* window = glfwCreateWindow(1200, 900, "test", NULL, NULL);

  if (window == NULL) {
    glfwTerminate();
    fprintf(stderr, "Window creation failed!\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // OpenGL 3.x/4.xの関数をロードする (glfwMakeContextCurrentの後でないといけない)
  const int version = gladLoadGL();
  if (version == 0) {
    fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
    return 1;
  }

  std::shared_ptr<ViewerModel> model = std::make_shared<ViewerModel>();
  ModelParser::parse(configFilePath, model);
  model->compileShaders();
  model->setMaskMode(Window::isMaskMode);

  Window::renderer = std::make_shared<Renderer>(&Window::WIN_WIDTH, &Window::WIN_HEIGHT, model);
  Window::resetCameraPose();

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.IniFilename = nullptr;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);

  SceneBuffer::setFrameBuffer(640, 480);

  std::cout << "Start window loop !" << std::endl;
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);

    {
      // OpenGL Rendering
      SceneBuffer::frameBuffer->Bind();
      if (Window::enabledRotationgMode) {
        Window::renderer->rotateModel(Window::ROTATE_ANIMATION_ANGLE, Window::cameraUp);
      }

      Window::renderer->paintGL();

      SceneBuffer::frameBuffer->Unbind();
    }

    // feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
      // Side Bar
      if (ImGui::BeginViewportSideBar("Demo window", ImGui::GetWindowViewport(), ImGuiDir_::ImGuiDir_Left, 200.0, true)) {
        ImGui::Button("Hello!");
        ImGui::End();
      }
    }

    {
      // Render OpenGL buffer
      ImVec2 viewportPos = ImGui::GetWindowViewport()->Pos;
      ImGui::SetNextWindowPos(ImVec2(viewportPos.x + 200.0, viewportPos.y));
      ImVec2 windowSize = ImVec2(ImGui::GetWindowViewport()->Size.x - 200.0, ImGui::GetWindowViewport()->Size.y);
      ImGui::SetNextWindowSizeConstraints(windowSize, windowSize);

      ImGui::Begin("My Scene");
      Window::WIN_WIDTH = ImGui::GetContentRegionAvail().x;
      Window::WIN_HEIGHT = ImGui::GetContentRegionAvail().y;

      SceneBuffer::windowSizeCallback(window, Window::WIN_WIDTH, Window::WIN_HEIGHT);
      glViewport(0, 0, Window::WIN_WIDTH, Window::WIN_HEIGHT);

      ImVec2 pos = ImGui::GetCursorScreenPos();

      ImGui::GetWindowDrawList()->AddImage(
          (void*)SceneBuffer::frameBuffer->getFrameTexture(),
          ImVec2(pos.x, pos.y),
          ImVec2(pos.x + Window::WIN_WIDTH, pos.y + Window::WIN_HEIGHT),
          ImVec2(0, 1),
          ImVec2(1, 0));

      ImGui::End();
    }

    // Render dear imgui into screen
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}