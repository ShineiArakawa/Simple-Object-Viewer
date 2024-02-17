#pragma once

#include <FileBrowser/ImGuiFileBrowser.h>

#include <ImGui.hpp>
#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <memory>
#include <string>

namespace gui {

class ObjectAddPopup {
 private:
  inline static const std::string TITLE = "Add Object";

  imgui_addons::ImGuiFileBrowser _fileDialog;
  std::shared_ptr<ViewerModel> _model;
  bool _isVisible = false;

  void openFileDialog();

 public:
  ObjectAddPopup(std::shared_ptr<ViewerModel>);
  ~ObjectAddPopup();

  void buildUI();
  void setVisible();
};

};  // namespace gui