#include <Window/ImGuiMainView.hpp>

ImGuiMainView::ImGuiMainView(GLFWwindow* mainWindow, std::shared_ptr<ViewerModel> sceneModel) {
  _sceneModel = sceneModel;

  // ====================================================================
  // Initialize Renderer
  // ====================================================================
  _renderer = std::make_shared<Renderer>(&ImGuiSceneView::WIN_WIDTH, &ImGuiSceneView::WIN_HEIGHT, _sceneModel);

  // ====================================================================
  // Initialize scene window
  // ====================================================================
  _sceneView = std::make_shared<ImGuiSceneView>(mainWindow, _renderer);

  // ====================================================================
  // Initialize popup
  // ====================================================================
  _objectAddDialog = std::make_shared<ObjectAddFileDialog>(_sceneModel);

  // ====================================================================
  // Initialize ImGui
  // ====================================================================
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  _io = &ImGui::GetIO();
  _io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  _io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  _io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  _io->ConfigWindowsMoveFromTitleBarOnly = true;
  _io->IniFilename = nullptr;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

ImGuiMainView::~ImGuiMainView() {}

void ImGuiMainView::destroy() {
  // ====================================================================
  // Clean up
  // ====================================================================
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void ImGuiMainView::paintMenuBar() {
  if (ImGui::BeginMainMenuBar()) {
    _menuBarHeight = ImGui::GetWindowHeight();

    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Add object", "Ctrl+O")) {
        _objectAddDialog->setVisible();
      }
      if (ImGui::MenuItem("Quit")) {
        std::cout << "Quit the program" << std::endl;
        moveOn = false;
        destroy();
        return;
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("help", "Ctrl+H")) {
      }
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

void ImGuiMainView::paintSideBar() {
  if (ImGui::BeginViewportSideBar("Demo window", ImGui::GetWindowViewport(), ImGuiDir_::ImGuiDir_Left, SIDEBAR_WIDTH, true)) {
    ImGui::SeparatorText("Rendering");

    // Render type
    static int renderType;
    static const char* renderTypeItems = "Normal\0Color\0Texture\0Vertex Normal\0";
    ImGui::Combo("Render Type", &renderType, renderTypeItems);
    _sceneModel->setRenderType(static_cast<Primitives::RenderType>(renderType));

    // Mask mode
    ImGui::Checkbox("Mask mode", &_sceneView->isMaskMode);
    _sceneModel->setMaskMode(_sceneView->isMaskMode);

    // Home position
    if (ImGui::Button("Reset Camera Pose")) {
      _sceneView->resetCameraPose();
    }

    // Rotation mode
    ImGui::Checkbox("Rotation mode", &_sceneView->enabledRotationgMode);

    // Background color
    const auto& arrayBackgroundRGBA = _sceneModel->getBackgroundColor();
    float backgroundRGBA[4] = {arrayBackgroundRGBA[0], arrayBackgroundRGBA[1], arrayBackgroundRGBA[2], arrayBackgroundRGBA[3]};
    ImGui::ColorEdit4("Background Color", &backgroundRGBA[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
    _sceneModel->setBackgroundColor(backgroundRGBA[0], backgroundRGBA[1], backgroundRGBA[2], backgroundRGBA[3]);

    ImGui::SeparatorText("Objects");
    if (ImGui::BeginTable("##Objects", 4, ImGuiTableFlags_Borders)) {
      ImGui::TableSetupColumn("Name");
      ImGui::TableSetupColumn("Type");
      ImGui::TableSetupColumn("Visible");
      ImGui::TableHeadersRow();

      for (int iObject = 0; iObject < _sceneModel->getNumObjects(); iObject++) {
        ImGui::TableNextRow();

        const auto object = _sceneModel->getObject(iObject);

        ImGui::TableNextColumn();
        ImGui::PushID(iObject * 4 + 0);
        ImGui::Text(object->getName().c_str());
        ImGui::PopID();

        ImGui::TableNextColumn();
        ImGui::PushID(iObject * 4 + 1);
        ImGui::Text(object->getObjectType().c_str());
        ImGui::PopID();

        ImGui::TableNextColumn();
        ImGui::PushID(iObject * 4 + 2);
        ImGui::Checkbox("##isVisible", object->getPointerToIsVisible());
        ImGui::PopID();

        ImGui::TableNextColumn();
        ImGui::PushID(iObject * 4 + 3);
        if (ImGui::Button("Remove")) {
          _sceneModel->removeObject(iObject);
        }
        ImGui::PopID();
      }

      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      if (ImGui::Button("Add ...")) {
        _objectAddDialog->setVisible();
      }
      ImGui::TableNextColumn();
      ImGui::TableNextColumn();
      ImGui::TableNextColumn();

      ImGui::EndTable();
    }

    ImGui::SeparatorText("Statistics");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / _io->Framerate, _io->Framerate);

    ImGui::End();
  }
}

void ImGuiMainView::paintSceneWindow() {
  const ImVec2 viewportPos = ImGui::GetWindowViewport()->Pos;
  ImGui::SetNextWindowPos(ImVec2(viewportPos.x + SIDEBAR_WIDTH, viewportPos.y + _menuBarHeight), ImGuiCond_Once);
  const ImVec2 sceneWindowSize = ImVec2(ImGui::GetWindowViewport()->Size.x - SIDEBAR_WIDTH, ImGui::GetWindowViewport()->Size.y - _menuBarHeight);
  ImGui::SetNextWindowSize(sceneWindowSize, ImGuiCond_Once);

  ImGui::Begin("Scene");

  _isForcusedOnScene = ImGui::IsWindowFocused();
  const ImVec2 sceneAreaSize = ImGui::GetContentRegionAvail();
  _sceneView->resizeGL(sceneAreaSize.x, sceneAreaSize.y);

  const ImVec2 sceneAreaOrigin = ImGui::GetCursorScreenPos();
  _sceneAreaMin = ImVec2(sceneAreaOrigin.x, sceneAreaOrigin.y);
  _sceneAreaMax = ImVec2(sceneAreaOrigin.x + sceneAreaSize.x, sceneAreaOrigin.y + sceneAreaSize.y);

  _wheelOffset = _io->MouseWheel;

  ImGui::GetWindowDrawList()->AddImage(
      (void*)_sceneView->getFrameBuffer()->getFrameTexture(),
      _sceneAreaMin,
      _sceneAreaMax,
      ImVec2(0, 1),
      ImVec2(1, 0));

  ImGui::End();
}

void ImGuiMainView::paint() {
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // ====================================================================
  // Generate main frame
  // ====================================================================
  {
    // Menu Bar
    paintMenuBar();

    // Side Bar
    paintSideBar();

    // Scene window
    paintSceneWindow();

    // Add object dialog
    _objectAddDialog->paint();
  }

  ImGui::Render();

  // ====================================================================
  // Render OpenGL scene
  // ====================================================================
  _sceneView->paintGL();

  // ====================================================================
  // Post process
  // ====================================================================
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  if (_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }

  glfwPollEvents();
}

void ImGuiMainView::listenEvent() {
  if (moveOn) {
    // ====================================================================
    // Event handling
    // ====================================================================
    {
      // Mouse on Scene
      const ImVec2 mousePos = ImGui::GetMousePos();
      const bool isMouseOnScene = (_sceneAreaMin.x <= mousePos.x) && (mousePos.x <= _sceneAreaMax.x) && (_sceneAreaMin.y <= mousePos.y) && (mousePos.y <= _sceneAreaMax.y);
      const ImVec2 relMousePos(mousePos.x - _sceneAreaMin.x, mousePos.y - _sceneAreaMin.y);

      _sceneView->mouseEvent(isMouseOnScene && _isForcusedOnScene, relMousePos);
      _sceneView->wheelEvent(isMouseOnScene && _isForcusedOnScene, _wheelOffset);
      _sceneView->motionEvent(isMouseOnScene && _isForcusedOnScene, relMousePos);
    }

    {
      // Keyboard
      const auto cameraDirection = glm::normalize(_sceneView->cameraLookAt - _sceneView->cameraPos);
      if (ImGui::IsKeyPressed(ImGuiKey_W)) {
        _sceneView->cameraPos += cameraDirection * _sceneView->CAMERA_MOVE_STEP;
        _renderer->setViewMat(glm::lookAt(_sceneView->cameraPos, _sceneView->cameraLookAt, _sceneView->cameraUp));
      } else if (ImGui::IsKeyPressed(ImGuiKey_A)) {
        const glm::vec3 moveVec = glm::cross(_sceneView->cameraUp, cameraDirection) * _sceneView->CAMERA_MOVE_STEP;
        _sceneView->cameraPos += moveVec;
        _sceneView->cameraLookAt += moveVec;
        _renderer->setViewMat(glm::lookAt(_sceneView->cameraPos, _sceneView->cameraLookAt, _sceneView->cameraUp));
      } else if (ImGui::IsKeyPressed(ImGuiKey_S)) {
        _sceneView->cameraPos += -cameraDirection * _sceneView->CAMERA_MOVE_STEP;
        _renderer->setViewMat(glm::lookAt(_sceneView->cameraPos, _sceneView->cameraLookAt, _sceneView->cameraUp));
      } else if (ImGui::IsKeyPressed(ImGuiKey_D)) {
        const glm::vec3 moveVec = glm::cross(cameraDirection, _sceneView->cameraUp) * _sceneView->CAMERA_MOVE_STEP;
        _sceneView->cameraPos += moveVec;
        _sceneView->cameraLookAt += moveVec;
        _renderer->setViewMat(glm::lookAt(_sceneView->cameraPos, _sceneView->cameraLookAt, _sceneView->cameraUp));
      } else if (ImGui::IsKeyPressed(ImGuiKey_Q)) {
        _renderer->rotateModel(_sceneView->MODEL_ROTATE_STEP, _sceneView->cameraUp);
      } else if (ImGui::IsKeyPressed(ImGuiKey_E)) {
        _renderer->rotateModel(-_sceneView->MODEL_ROTATE_STEP, _sceneView->cameraUp);
      }
    }
  }
}
