#include <Window/ObjectAddPopup.hpp>

FileDialog::FileDialog(const std::string label, MODE mode) : _label(label) {
  _mode = mode;
  _fileDialog = std::make_shared<imgui_addons::ImGuiFileBrowser>();
}

FileDialog::~FileDialog() {}

void FileDialog::setup(char* textBUffer, const std::string extensions) {
  _textBuffer = textBUffer;
  _extensions = extensions;
}

void FileDialog::paint() {
  if (isVisible) {
    ImGui::OpenPopup(_label.c_str());
  }

  if (_fileDialog->showFileDialog(_label, _mode, ImVec2(700, 600), _extensions)) {
    strcpy(_textBuffer, (char*)_fileDialog->selected_path.data());
  }

  isVisible = false;
}

ObjectAddFileDialog::ObjectAddFileDialog(std::shared_ptr<ViewerModel> model) : _model(model) {
  _fileDialog = std::make_shared<FileDialog>("Open file", FileDialog::MODE::OPEN);

  _objectTypes = ""s;
  _objectTypes += Object::KEY_MODEL_OBJECT + "\0"s;
  _objectTypes += Box::KEY_MODEL_BOX + "\0"s;
  _objectTypes += Terrain::KEY_MODEL_TERRAIN + "\0"s;
  _objectTypes += Background::KEY_MODEL_BACKGROUND + "\0"s;
  _objectTypes += Sphere::KEY_MODEL_SPHERE + "\0"s;
}

ObjectAddFileDialog::~ObjectAddFileDialog() {}

void ObjectAddFileDialog::paint() {
  static int objectTypeID = 0;
  static char objName[256];
  static char objFilePath[256];
  static char textureFilePath[256];
  static char heightMapFilePath[256];
  static float offsetXYZ[3] = {0.0f, 0.0f, 0.0f};
  static float scale = 1.0f;
  static float scaleXYZ[3] = {1.0f, 1.0f, 1.0f};
  static int nDivs = 100;

  if (_isVisible) {
    ImGui::Begin(TITLE.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SeparatorText("1. Select the object type");
    ImGui::Combo("Object Type", &objectTypeID, _objectTypes.c_str());

    ImGui::SeparatorText("2. Set each attribute");
    // ========================================================================================================================
    // Create GUI for attributes
    // ========================================================================================================================
    if (objectTypeID == 0) {
      // ====================================================================
      // Object
      // ====================================================================
      ImGui::InputText("Obj Name", objName, 256);

      ImGui::InputText("Obj file path", objFilePath, 256);
      ImGui::SameLine();
      if (ImGui::Button("Browse Obj")) {
        _fileDialog->isVisible = true;
        _fileDialog->setup(objFilePath, ".obj");
      }

      ImGui::InputText("Texture file path", textureFilePath, 256);
      ImGui::SameLine();
      if (ImGui::Button("Browse Texture")) {
        _fileDialog->isVisible = true;
        _fileDialog->setup(textureFilePath, ".png,.jpg");
      }

      ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ);

      ImGui::InputFloat("Scale", &scale);
    } else if (objectTypeID == 1) {
      // ====================================================================
      // Box
      // ====================================================================
      ImGui::InputText("Obj Name", objName, 256);
      ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ);
      ImGui::InputFloat3("Scale (X, Y, Z)", scaleXYZ);
    } else if (objectTypeID == 2) {
      // ====================================================================
      // Terrain
      // ====================================================================
      ImGui::InputText("Obj Name", objName, 256);
      ImGui::InputText("Height map file path", heightMapFilePath, 256);
      ImGui::SameLine();
      if (ImGui::Button("Browse Obj")) {
        _fileDialog->isVisible = true;
        _fileDialog->setup(heightMapFilePath, ".png,.jpg");
      }
      ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ);
      ImGui::InputFloat3("Scale (X, Y, H)", scaleXYZ);
    } else if (objectTypeID == 3) {
      // ====================================================================
      // Background
      // ====================================================================
      ImGui::InputText("Obj Name", objName, 256);
      ImGui::InputText("Texture file path", textureFilePath, 256);
      ImGui::SameLine();
      if (ImGui::Button("Browse texture")) {
        _fileDialog->isVisible = true;
        _fileDialog->setup(textureFilePath, ".png,.jpg");
      }
    } else if (objectTypeID == 4) {
      // ====================================================================
      // Sphere
      // ====================================================================
      ImGui::InputText("Obj Name", objName, 256);
      ImGui::InputInt("num divisions", &nDivs);
      ImGui::InputFloat3("Offset (X, Y, Z)", offsetXYZ);
      ImGui::InputFloat3("Scale (X, Y, Z)", scaleXYZ);
    }

    // ========================================================================================================================
    // Create GUI button
    // ========================================================================================================================
    ImGui::Separator();
    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();  // close
      _isVisible = false;
      _message.clear();
      _errorMessage.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("Add")) {
      // ========================================================================================================================
      // Create objects
      // ========================================================================================================================
      Model::t_object newObject = nullptr;
      std::string strObjName(objName);
      std::string strObjFilePath(objFilePath);
      std::string strTexFilePath(textureFilePath);
      std::string strHeightMapFilePath(heightMapFilePath);
      std::string strTextureFilePath(textureFilePath);

      try {
        _message = "Initializing ...";

        if (objectTypeID == 0) {
          // ====================================================================
          // Object
          // ====================================================================
          auto object = std::make_shared<Object>(strObjFilePath, offsetXYZ[0], offsetXYZ[1], offsetXYZ[2], scale);
          if (!strTexFilePath.empty()) {
            object->loadTexture(strTexFilePath);
          }

          newObject = object;
        } else if (objectTypeID == 1) {
          // ====================================================================
          // Box
          // ====================================================================
          newObject = std::make_shared<Box>(offsetXYZ[0], offsetXYZ[1], offsetXYZ[2], scaleXYZ[0], scaleXYZ[1], scaleXYZ[2]);
        } else if (objectTypeID == 2) {
          // ====================================================================
          // Terrain
          // ====================================================================
          newObject = std::make_shared<Terrain>(strHeightMapFilePath, offsetXYZ[0], offsetXYZ[1], offsetXYZ[2], scaleXYZ[0], scaleXYZ[1], scaleXYZ[2]);
        } else if (objectTypeID == 3) {
          // ====================================================================
          // Backgound
          // ====================================================================
          newObject = std::make_shared<Background>(strTextureFilePath);
        } else if (objectTypeID == 4) {
          // ====================================================================
          // Sphere
          // ====================================================================
          newObject = std::make_shared<Sphere>(nDivs, offsetXYZ[0], offsetXYZ[1], offsetXYZ[2], scaleXYZ[0], scaleXYZ[1], scaleXYZ[2]);
        }

        if (newObject != nullptr) {
          newObject->setName(strObjName);
          newObject->setShader(_model->getShaderID());
          newObject->initVAO();
          _model->addObject(std::move(newObject));
          _errorMessage.clear();
          _message = "Success";
        }
      } catch (const std::exception& error) {
        std::cerr << "[Error] See below messages." << std::endl;
        std::cerr << error.what() << std::endl;
        _errorMessage = "Failed to add a new object. Check attributes.";
      }
    }

    if (!_errorMessage.empty()) {
      ImGui::Separator();
      ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), _errorMessage.c_str());
    } else if (!_message.empty()) {
      ImGui::Separator();
      ImGui::Text(_message.c_str());
    }

    ImGui::End();
  }

  _fileDialog->paint();
}

void ObjectAddFileDialog::setVisible() {
  _isVisible = true;
}
