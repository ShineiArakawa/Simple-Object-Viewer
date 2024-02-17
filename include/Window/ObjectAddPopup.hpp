#pragma once

#include <FileBrowser/ImGuiFileBrowser.h>

#include <ImGui.hpp>
#include <Model/Object.hpp>
#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <map>
#include <memory>
#include <string>
#include <variant>

class FileDialog {
  std::shared_ptr<imgui_addons::ImGuiFileBrowser> _fileDialog = nullptr;
  char* _textBuffer = nullptr;
  std::string _extensions = "";

 public:
  bool isVisible = false;

  FileDialog();
  ~FileDialog();

  void setup(char*, const std::string);
  void buildUI();
};

class ObjectAddFileDialog {
 private:
  using Attrib_t = std::variant<int, float, std::string>;

  inline static const std::string TITLE = "Add Object";

  std::shared_ptr<FileDialog> _fileDialog;
  std::shared_ptr<ViewerModel> _model;
  bool _isVisible = false;
  std::map<std::string, Attrib_t> _attributeBuffer;

 public:
  ObjectAddFileDialog(std::shared_ptr<ViewerModel>);
  ~ObjectAddFileDialog();

  void buildUI();
  void setVisible();
};
