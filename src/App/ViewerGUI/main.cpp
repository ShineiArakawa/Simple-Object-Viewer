#include <App/ViewerGUIMain.hpp>

int main(int argc, char** argv) {
  // ====================================================================
  // Parse arguments
  // ====================================================================
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
    configFilePath = FileUtil::absPath(DEFAULT_CONFIG_PATH);
    std::cout << "The config file path was not specified. Continue with the default config path: " << configFilePath << std::endl;
  }

  // Check config file
  if (!FileUtil::exists(configFilePath)) {
    std::cerr << "Failed to open the config file. Please check the arguments." << std::endl;
    std::cerr << "args: {config_file}" << std::endl;
    std::exit(1);
  }

  // ====================================================================
  // Initialize GLFW window
  // ====================================================================
  if (glfwInit() == GLFW_FALSE) {
    fprintf(stderr, "Initialization failed!\n");
    return 1;
  }

  GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

  if (window == NULL) {
    glfwTerminate();
    fprintf(stderr, "Window creation failed!\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // ====================================================================
  // Initialize GLAD
  // ====================================================================
  if (glfwInit() == GLFW_FALSE) {
    fprintf(stderr, "Initialization failed!\n");
    return 1;
  }
  if (gladLoadGL() == 0) {
    fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
    return 1;
  }

  // ====================================================================
  // Initialize Model
  // ====================================================================
  model = std::make_shared<ViewerModel>();
  ModelParser::parse(configFilePath, model);
  model->compileShaders();
  model->setMaskMode(ImGuiSceneView::isMaskMode);

  // ====================================================================
  // Initialize Renderer
  // ====================================================================
  renderer = std::make_shared<Renderer>(&ImGuiSceneView::WIN_WIDTH, &ImGuiSceneView::WIN_HEIGHT, model);

  // ====================================================================
  // Initialize scene window
  // ====================================================================
  scene = std::make_shared<ImGuiSceneView>(window, renderer);

  // ====================================================================
  // Initialize popup
  // ====================================================================
  auto objectAddPopup = std::make_shared<gui::ObjectAddPopup>(model);

  // ====================================================================
  // Initialize ImGui
  // ====================================================================
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.IniFilename = nullptr;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  static float menuBarHeight;
  static bool isForcusedOnScene;
  static float wheelOffset;
  static ImVec2 sceneAreaMin;
  static ImVec2 sceneAreaMax;

  // ====================================================================
  // Start main loop
  // ====================================================================
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ====================================================================
    // Generate main frame
    // ====================================================================
    {
      // Menu Bar
      if (ImGui::BeginMainMenuBar()) {
        menuBarHeight = ImGui::GetWindowHeight();
        if (ImGui::BeginMenu("File")) {
          if (ImGui::MenuItem("Add object", "Ctrl+O")) {
            objectAddPopup->setVisible();
          }
          if (ImGui::MenuItem("Quit")) {
            std::cout << "Quit the program" << std::endl;
            break;
          }
          ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
      }

      // Popup
      objectAddPopup->buildUI();

      // Side Bar
      if (ImGui::BeginViewportSideBar("Demo window", ImGui::GetWindowViewport(), ImGuiDir_::ImGuiDir_Left, SIDEBAR_WIDTH, true)) {
        // Background color
        const auto& arrayBackgroundRGBA = model->getBackgroundColor();
        float backgroundRGBA[4] = {arrayBackgroundRGBA[0], arrayBackgroundRGBA[1], arrayBackgroundRGBA[2], arrayBackgroundRGBA[3]};
        ImGui::ColorEdit4("Background Color", &backgroundRGBA[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
        model->setBackgroundColor(backgroundRGBA[0], backgroundRGBA[1], backgroundRGBA[2], backgroundRGBA[3]);

        // Render type
        static int renderType;
        static const char* renderTypeItems = "Normal\0Color\0Texture\0Vertex Normal\0";
        ImGui::Combo("Render Type", &renderType, renderTypeItems);
        model->setRenderType(static_cast<Primitives::RenderType>(renderType));

        // Mask mode
        ImGui::Checkbox("Mask mode", &scene->isMaskMode);
        model->setMaskMode(scene->isMaskMode);

        // Home position
        if (ImGui::Button("Reset Camera Pose")) {
          scene->resetCameraPose();
        }

        // Rotation mode
        ImGui::Checkbox("Rotation mode", &scene->enabledRotationgMode);

        ImGui::BeginChild("FPS");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::EndChild();

        ImGui::End();
      }
    }

    {
      // Render OpenGL buffer
      const ImVec2 viewportPos = ImGui::GetWindowViewport()->Pos;
      ImGui::SetNextWindowPos(ImVec2(viewportPos.x + SIDEBAR_WIDTH, viewportPos.y + menuBarHeight), ImGuiCond_Once);
      const ImVec2 sceneWindowSize = ImVec2(ImGui::GetWindowViewport()->Size.x - SIDEBAR_WIDTH, ImGui::GetWindowViewport()->Size.y - menuBarHeight);
      ImGui::SetNextWindowSize(sceneWindowSize, ImGuiCond_Once);

      ImGui::Begin("Scene");
      {
        isForcusedOnScene = ImGui::IsWindowFocused();
        const ImVec2 sceneAreaSize = ImGui::GetContentRegionAvail();
        scene->resizeGL(sceneAreaSize.x, sceneAreaSize.y);

        const ImVec2 sceneAreaOrigin = ImGui::GetCursorScreenPos();
        sceneAreaMin = ImVec2(sceneAreaOrigin.x, sceneAreaOrigin.y);
        sceneAreaMax = ImVec2(sceneAreaOrigin.x + sceneAreaSize.x, sceneAreaOrigin.y + sceneAreaSize.y);

        wheelOffset = io.MouseWheel;

        ImGui::GetWindowDrawList()->AddImage(
            (void*)scene->getFrameBuffer()->getFrameTexture(),
            sceneAreaMin,
            sceneAreaMax,
            ImVec2(0, 1),
            ImVec2(1, 0));
      }
      ImGui::End();
    }
    ImGui::Render();

    // ====================================================================
    // Event handling
    // ====================================================================
    {
      // Mouse on Scene
      const ImVec2 mousePos = ImGui::GetMousePos();
      const bool isMouseOnScene = (sceneAreaMin.x <= mousePos.x) && (mousePos.x <= sceneAreaMax.x) && (sceneAreaMin.y <= mousePos.y) && (mousePos.y <= sceneAreaMax.y);
      const ImVec2 relMousePos(mousePos.x - sceneAreaMin.x, mousePos.y - sceneAreaMin.y);

      scene->mouseEvent(isMouseOnScene && isForcusedOnScene, relMousePos);
      scene->wheelEvent(isMouseOnScene && isForcusedOnScene, wheelOffset);
      scene->motionEvent(isMouseOnScene && isForcusedOnScene, relMousePos);
    }

    {
      // Keyboard
      const auto cameraDirection = glm::normalize(scene->cameraLookAt - scene->cameraPos);
      if (ImGui::IsKeyPressed(ImGuiKey_W)) {
        scene->cameraPos += cameraDirection * scene->CAMERA_MOVE_STEP;
        renderer->setViewMat(glm::lookAt(scene->cameraPos, scene->cameraLookAt, scene->cameraUp));
      } else if (ImGui::IsKeyPressed(ImGuiKey_A)) {
        const glm::vec3 moveVec = glm::cross(scene->cameraUp, cameraDirection) * scene->CAMERA_MOVE_STEP;
        scene->cameraPos += moveVec;
        scene->cameraLookAt += moveVec;
        renderer->setViewMat(glm::lookAt(scene->cameraPos, scene->cameraLookAt, scene->cameraUp));
      } else if (ImGui::IsKeyPressed(ImGuiKey_S)) {
        scene->cameraPos += -cameraDirection * scene->CAMERA_MOVE_STEP;
        renderer->setViewMat(glm::lookAt(scene->cameraPos, scene->cameraLookAt, scene->cameraUp));
      } else if (ImGui::IsKeyPressed(ImGuiKey_D)) {
        const glm::vec3 moveVec = glm::cross(cameraDirection, scene->cameraUp) * scene->CAMERA_MOVE_STEP;
        scene->cameraPos += moveVec;
        scene->cameraLookAt += moveVec;
        renderer->setViewMat(glm::lookAt(scene->cameraPos, scene->cameraLookAt, scene->cameraUp));
      } else if (ImGui::IsKeyPressed(ImGuiKey_Q)) {
        renderer->rotateModel(scene->MODEL_ROTATE_STEP, scene->cameraUp);
      } else if (ImGui::IsKeyPressed(ImGuiKey_E)) {
        renderer->rotateModel(-scene->MODEL_ROTATE_STEP, scene->cameraUp);
      }
    }
    // ====================================================================
    // Render OpenGL scene
    // ====================================================================
    scene->paintGL();

    // ====================================================================
    // Post process
    // ====================================================================
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // ====================================================================
  // Clean up
  // ====================================================================
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
