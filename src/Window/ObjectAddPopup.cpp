#include <Window/ObjectAddPopup.hpp>

FileDialog::FileDialog() {
  _fileDialog = std::make_shared<imgui_addons::ImGuiFileBrowser>();
}

FileDialog::~FileDialog() {}

void FileDialog::setup(char* textBUffer, const std::string extensions) {
  _textBuffer = textBUffer;
  _extensions = extensions;
}

void FileDialog::buildUI() {
  if (isVisible) {
    ImGui::OpenPopup("Open File");
  }

  if (_fileDialog->showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 600), _extensions)) {
    strcpy(_textBuffer, (char*)_fileDialog->selected_path.data());
    std::cout << _textBuffer << std::endl;
  }
}

ObjectAddFileDialog::ObjectAddFileDialog(std::shared_ptr<ViewerModel> model) : _model(model) {
  _fileDialog = std::make_shared<FileDialog>();
}

ObjectAddFileDialog::~ObjectAddFileDialog() {}

void ObjectAddFileDialog::buildUI() {
  static int objectTypeID = 0;
  static char objName[256];
  static char objFilePath[256];
  static char textureFilePath[256];
  static float offset[3] = {0.0f, 0.0f, 0.0f};
  static float scale = 1.0f;

  if (_isVisible) {
    ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_Once);
    ImGui::Begin(TITLE.c_str());

    const char* objectTypes = "Object\0";

    ImGui::SeparatorText("1. Select the object type");
    ImGui::Combo("Object Type", &objectTypeID, objectTypes);

    ImGui::SeparatorText("2. Set each attributes");
    if (objectTypeID == 0) {
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

      ImGui::InputFloat3("Offset (X, Y, Z)", offset);

      ImGui::InputFloat("Scale", &scale);
    }

    ImGui::Separator();
    if (ImGui::Button("Cancel")) {
      ImGui::CloseCurrentPopup();  // close
      _isVisible = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Add")) {
      ImGui::CloseCurrentPopup();  // close
      _isVisible = false;

      if (objectTypeID == 0) {
        std::string strObjFilePath(objFilePath);
        std::string strTexFilePath(textureFilePath);
        std::string strObjName(objName);

        printf("ObjFilePath=%s\n", strObjFilePath.c_str());
        printf("TexFilePath=%s\n", strTexFilePath.c_str());
        printf("Offset: (%f, %f, %f)\n", offset[0], offset[1], offset[2]);
        printf("Scale: %f\n", scale);
        fflush(stdout);

        std::shared_ptr<Object> object = std::make_shared<Object>(strObjFilePath, offset[0], offset[1], offset[2], scale);

        if (!strTexFilePath.empty()) {
          object->loadTexture(strTexFilePath);
        }

        object->setName(strObjName);
        _model->addObject(std::move(object));
        _model->compileShaders();
        _model->initVAO();
      }
    }

    ImGui::End();
  }

  _fileDialog->buildUI();
  _fileDialog->isVisible = false;
}

void ObjectAddFileDialog::setVisible() {
  _isVisible = true;
}
