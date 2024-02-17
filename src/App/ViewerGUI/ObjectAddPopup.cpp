#include <App/ObjectAddPopup.hpp>

namespace gui {

ObjectAddPopup::ObjectAddPopup(std::shared_ptr<ViewerModel> model) : _model(model) {
}

ObjectAddPopup::~ObjectAddPopup() {}

void ObjectAddPopup::buildUI() {
  bool open = false;

  if (_isVisible) {
    ImGui::SetNextWindowSize(ImVec2(400, 800), ImGuiCond_Once);
    ImGui::Begin(TITLE.c_str());

    int objectTypeID = 0;
    const char* objectTypes = "Object\0";

    ImGui::SeparatorText("1. Select the object type");
    ImGui::Combo("Object Type", &objectTypeID, objectTypes);

    ImGui::SeparatorText("2. Set each attributes");
    if (objectTypeID == 0) {
      std::string objFilePath = "";
      ImGui::InputText("Obj file path", (char*)objFilePath.data(), objFilePath.capacity());
      ImGui::SameLine();
      if (ImGui::Button("Browse")) {
        open = true;
      }
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
    }

    ImGui::End();
  }

  if (open) {
    ImGui::OpenPopup("Open File");
  }

  if (_fileDialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 500), ".rar,.zip,.7z")) {
    std::cout << _fileDialog.selected_fn << std::endl;    // The name of the selected file or directory in case of Select Directory dialog mode
    std::cout << _fileDialog.selected_path << std::endl;  // The absolute path to the selected file
  }
}

void ObjectAddPopup::setVisible() {
  _isVisible = true;
}

}  // namespace gui