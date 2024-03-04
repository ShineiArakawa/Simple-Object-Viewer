#include <Window/ImGuiMainView.hpp>

ImGuiMainView::ImGuiMainView(GLFWwindow* mainWindow, std::shared_ptr<ViewerModel> sceneModel) {
  _sceneModel = sceneModel;

  // ====================================================================
  // Initialize scene window
  // ====================================================================
  _sceneView = std::make_shared<ImGuiSceneView>(mainWindow, _sceneModel);

  // ====================================================================
  // Initialize depth scene model
  // ====================================================================
  _depthSceneModel = std::make_shared<ViewerModel>();
  _depthSceneModel->compileShaders(true);

  // ====================================================================
  // Initialize depth scene window
  // ====================================================================
  _depthSceneView = std::make_shared<ImGuiSceneView>(mainWindow, _depthSceneModel);

  auto background = std::make_shared<Background>(_sceneView->getRenderer()->getDepthRenderer()->getDepthMapId());

  _depthSceneModel->addBackground(background);

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

  NFD_Init();
}

ImGuiMainView::~ImGuiMainView() {}

void ImGuiMainView::destroy() {
  // ====================================================================
  // Clean up
  // ====================================================================
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  NFD_Quit();
}

void ImGuiMainView::paintMenuBar() {
  if (ImGui::BeginMainMenuBar()) {
    _menuBarHeight = ImGui::GetWindowHeight();

    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Quit")) {
        LOG_INFO("Quit the program");
        moveOn = false;
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
  if (ImGui::BeginViewportSideBar("Settings", ImGui::GetWindowViewport(), ImGuiDir_Left, SIDEBAR_WIDTH, true)) {
    // ========================================================================================
    // Rendering section
    // ========================================================================================
    if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
      // Render type
      static int renderType;
      static const char* renderTypeItems = "Normal\0Color\0Texture\0Vertex Normal\0Shading\0Shading with texture\0Material\0";
      ImGui::Combo("Render Type", &renderType, renderTypeItems);
      _sceneModel->setRenderType(static_cast<Primitives::RenderType>(renderType));

      // Mask mode
      ImGui::Checkbox("Mask mode", &_sceneView->isMaskMode);
      _sceneModel->setMaskMode(_sceneView->isMaskMode);

      // Background color
      const auto& arrayBackgroundRGBA = _sceneModel->getBackgroundColor();
      float backgroundRGBA[4] = {arrayBackgroundRGBA.x, arrayBackgroundRGBA.y, arrayBackgroundRGBA.z, arrayBackgroundRGBA.w};
      ImGui::ColorEdit4("Background Color", &backgroundRGBA[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
      _sceneModel->setBackgroundColor(backgroundRGBA[0], backgroundRGBA[1], backgroundRGBA[2], backgroundRGBA[3]);

      // Axes cone
      static bool isShownAxesCone = false;
      ImGui::Checkbox("Show axes cone", &isShownAxesCone);
      _sceneModel->setAxesConeState(isShownAxesCone);

      // Wire frame mode
      static int wireFrameMode;
      static const char* wireFrameModeItems = "OFF\0ON\0Wire frame only\0";
      ImGui::Combo("Wire Frame", &wireFrameMode, wireFrameModeItems);
      _sceneModel->setWireFrameMode(static_cast<Primitives::WireFrameMode>(wireFrameMode));

      // Wire frame width
      float wireFrameWidth = _sceneModel->getWireFrameWidth();
      ImGui::DragFloat("Wire Frame Width", &wireFrameWidth, 0.001f, 0.0f, 0.1f, FLOAT_FORMAT);
      _sceneModel->setWireFrameWidth(wireFrameWidth);

      // Wire frame color
      const auto& arrayWireFrameColor = _sceneModel->getWireFrameColor();
      float wireFrameColor[3] = {arrayWireFrameColor.x, arrayWireFrameColor.y, arrayWireFrameColor.z};
      ImGui::ColorEdit3("Wire Frame Color", &wireFrameColor[0], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
      _sceneModel->setWireFrameColor(wireFrameColor[0], wireFrameColor[1], wireFrameColor[2]);

      // Bump map
      ImGui::Checkbox("Bunm map", &_sceneView->isEnabledNormalMap);
      _sceneModel->setIsEnabledNormalMap(_sceneView->isEnabledNormalMap);

      // Shadow mapping
      ImGui::Checkbox("Shadow mapping", &_sceneView->isEnabledShadowMapping);
      _sceneModel->setIsEnabledShadowMapping(_sceneView->isEnabledShadowMapping);
    }

    // ========================================================================================
    // Animation section
    // ========================================================================================
    if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen)) {
      // Rotation Angle
      ImGui::DragFloat("Rotation Angle", &_sceneView->rotateAnimationAngle, 0.1f, 0.0f, 360.0f, FLOAT_FORMAT);

      // Rotation mode
      ImGui::Checkbox("Rotate Model", &_sceneView->enabledModelRotationMode);

      // Rotation mode
      ImGui::Checkbox("Rotate Light", &_sceneView->enabledLightRotationMode);

      // Home Camera position
      if (ImGui::Button("Reset Camera Pose")) {
        _sceneView->resetCameraPose();
      }

      // Home Light position
      ImGui::SameLine();
      if (ImGui::Button("Reset Light Pose")) {
        _sceneView->resetLightPose();
      }
    }

    // ========================================================================================
    // Lighting section
    // ========================================================================================
    if (ImGui::CollapsingHeader("Lighting", ImGuiTreeNodeFlags_DefaultOpen)) {
      // Specular
      float shininess = _sceneModel->getShininess();
      ImGui::InputFloat("Specular", &shininess, 1.0f, 0.0f, FLOAT_FORMAT);
      _sceneModel->setShiniess(shininess);

      // Light pos
      const auto& arrayLightPos = _sceneModel->getLightPos();
      float lightPos[3] = {arrayLightPos.x, arrayLightPos.y, arrayLightPos.z};
      ImGui::InputFloat3("Light position", lightPos, FLOAT_FORMAT);
      _sceneModel->setLightPosition(lightPos[0], lightPos[1], lightPos[2]);

      // Ambient intensity
      float ambientIntensity = _sceneModel->getAmbientIntensity();
      ImGui::DragFloat("Ambient intensity", &ambientIntensity, 0.001f, 0.0f, 1.0f, FLOAT_FORMAT);
      _sceneModel->setAmbientIntensity(ambientIntensity);
    }

    // ========================================================================================
    // Objects section
    // ========================================================================================
    if (ImGui::CollapsingHeader("Objects", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::SeparatorText("Registered Objects");
      if (ImGui::BeginTable("##Registered Objects", 4, ImGuiTableFlags_Borders)) {
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
          if (object->getObjectType() != AxesCone::KEY_MODEL_AXES_CONE) {
            ImGui::PushID(iObject * 4 + 3);
            if (ImGui::Button("Remove")) {
              _sceneModel->removeObject(iObject);
            }
            ImGui::PopID();
          }
        }

        bool* backgoundFlags = (bool*)calloc(_sceneModel->getNumBackgrounds(), sizeof(bool));
        if (_sceneModel->getNumBackgrounds() > 0) {
          backgoundFlags[_sceneModel->getBackgroundIDtoDraw()] = true;
        }

        for (int iBackground = 0; iBackground < _sceneModel->getNumBackgrounds(); iBackground++) {
          ImGui::TableNextRow();

          const auto background = _sceneModel->getBackground(iBackground);

          ImGui::TableNextColumn();
          ImGui::PushID((_sceneModel->getNumObjects() + iBackground) * 4 + 0);
          ImGui::Text(background->getName().c_str());
          ImGui::PopID();

          ImGui::TableNextColumn();
          ImGui::PushID((_sceneModel->getNumObjects() + iBackground) * 4 + 1);
          ImGui::Text(background->getObjectType().c_str());
          ImGui::PopID();

          ImGui::TableNextColumn();
          ImGui::PushID((_sceneModel->getNumObjects() + iBackground) * 4 + 2);
          ImGui::Checkbox("##isVisible", &backgoundFlags[iBackground]);
          ImGui::PopID();

          ImGui::TableNextColumn();
          ImGui::PushID((_sceneModel->getNumObjects() + iBackground) * 4 + 3);
          if (ImGui::Button("Remove")) {
            _sceneModel->removeBackground(iBackground);
          }
          ImGui::PopID();
        }

        for (int iBackground = 0; iBackground < _sceneModel->getNumBackgrounds(); iBackground++) {
          if (backgoundFlags[iBackground] && iBackground != _sceneModel->getBackgroundIDtoDraw()) {
            _sceneModel->setBackgroundIDtoDraw(iBackground);
            break;
          }
        }

        ImGui::EndTable();
      }

      ImGui::SeparatorText("Add object");
      _objectAddDialog->paint();
    }

    // ========================================================================================
    // Screen shot section
    // ========================================================================================
    if (ImGui::CollapsingHeader("Screen shot", ImGuiTreeNodeFlags_DefaultOpen)) {
      static char screenshotFilePath[256];
      ImGui::InputText("##Save to", screenshotFilePath, 256);
      ImGui::SameLine();
      if (ImGui::Button("Browse")) {
        nfdchar_t* outPath;
        nfdfilteritem_t filterItem[1] = {{"Image", "png,jpg"}};
        nfdresult_t result = NFD_SaveDialog(&outPath, filterItem, 1, NULL, "screenshot.png");

        if (result == NFD_OKAY) {
          strcpy(screenshotFilePath, outPath);
        }
      }
      ImGui::SameLine();
      if (ImGui::Button("Save")) {
        std::string strScreenshotFilePath(screenshotFilePath);
        std::cout << "Save screen shot to " << strScreenshotFilePath << std::endl;
        _sceneView->saveScreenShot(strScreenshotFilePath);
      }
    }

    // ========================================================================================
    // Statistics section
    // ========================================================================================
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
      reinterpret_cast<void*>(_sceneView->getFrameBuffer()->getFrameTexture()),
      _sceneAreaMin,
      _sceneAreaMax,
      ImVec2(0, 1),
      ImVec2(1, 0));

  ImGui::End();
}

void ImGuiMainView::paintDepthSceneWindow() {
  if (_sceneView->isEnabledShadowMapping && _depthSceneView != nullptr) {
    const ImVec2 viewportPos = ImGui::GetWindowViewport()->Pos;
    ImGui::SetNextWindowPos(ImVec2(viewportPos.x + SIDEBAR_WIDTH, viewportPos.y + _menuBarHeight), ImGuiCond_Once);
    const ImVec2 sceneWindowSize = ImVec2(ImGui::GetWindowViewport()->Size.x - SIDEBAR_WIDTH, ImGui::GetWindowViewport()->Size.y - _menuBarHeight);
    ImGui::SetNextWindowSize(sceneWindowSize, ImGuiCond_Once);

    ImGui::Begin("Depth");

    const ImVec2 sceneAreaSize = ImGui::GetContentRegionAvail();
    const ImVec2 sceneAreaOrigin = ImGui::GetCursorScreenPos();
    const ImVec2 sceneAreaMin = ImVec2(sceneAreaOrigin.x, sceneAreaOrigin.y);
    const ImVec2 sceneAreaMax = ImVec2(sceneAreaOrigin.x + sceneAreaSize.x, sceneAreaOrigin.y + sceneAreaSize.y);

    _depthSceneView->resizeGL(sceneAreaSize.x, sceneAreaSize.y);

    ImGui::GetWindowDrawList()->AddImage(
        reinterpret_cast<void*>(_depthSceneView->getFrameBuffer()->getFrameTexture()),
        sceneAreaMin,
        sceneAreaMax,
        ImVec2(0, 1),
        ImVec2(1, 0));

    ImGui::End();
  }
}

void ImGuiMainView::paint() {
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

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

    // Depth scene window
    paintDepthSceneWindow();
  }

  ImGui::Render();

  // ====================================================================
  // Render OpenGL scene
  // ====================================================================
  _sceneView->paintGL();

  if (_sceneView->isEnabledShadowMapping && _depthSceneView != nullptr) {
    _depthSceneView->paintGL();
  }

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
        // Camera move forward
        _sceneView->cameraPos += cameraDirection * _sceneView->CAMERA_MOVE_STEP;
        _sceneView->getRenderer()->setViewMat(glm::lookAt(_sceneView->cameraPos, _sceneView->cameraLookAt, _sceneView->cameraUp));
      } else if (ImGui::IsKeyPressed(ImGuiKey_A)) {
        // Camera move left
        const glm::vec3 moveVec = glm::cross(_sceneView->cameraUp, cameraDirection) * _sceneView->CAMERA_MOVE_STEP;
        _sceneView->cameraPos += moveVec;
        _sceneView->cameraLookAt += moveVec;
        _sceneView->getRenderer()->setViewMat(glm::lookAt(_sceneView->cameraPos, _sceneView->cameraLookAt, _sceneView->cameraUp));
      } else if (ImGui::IsKeyPressed(ImGuiKey_S)) {
        // Camera move backward
        _sceneView->cameraPos += -cameraDirection * _sceneView->CAMERA_MOVE_STEP;
        _sceneView->getRenderer()->setViewMat(glm::lookAt(_sceneView->cameraPos, _sceneView->cameraLookAt, _sceneView->cameraUp));
      } else if (ImGui::IsKeyPressed(ImGuiKey_D)) {
        // Camera move right
        const glm::vec3 moveVec = glm::cross(cameraDirection, _sceneView->cameraUp) * _sceneView->CAMERA_MOVE_STEP;
        _sceneView->cameraPos += moveVec;
        _sceneView->cameraLookAt += moveVec;
        _sceneView->getRenderer()->setViewMat(glm::lookAt(_sceneView->cameraPos, _sceneView->cameraLookAt, _sceneView->cameraUp));
      } else if (ImGui::IsKeyPressed(ImGuiKey_Q)) {
        // Camera rotate left
        _sceneView->getRenderer()->rotateModel(_sceneView->MODEL_ROTATE_STEP, _sceneView->cameraUp);
      } else if (ImGui::IsKeyPressed(ImGuiKey_E)) {
        // Camera rotate right
        _sceneView->getRenderer()->rotateModel(-_sceneView->MODEL_ROTATE_STEP, _sceneView->cameraUp);
      } else if (ImGui::IsKeyPressed(ImGuiKey_Home)) {
        _sceneView->resetCameraPose();
        _sceneView->resetLightPose();
      }
    }
  }
}
