#pragma once

#include <ImGuiFileBrowser.h>

#include <ImGui.hpp>
#include <Model/Background.hpp>
#include <Model/Box.hpp>
#include <Model/Object.hpp>
#include <Model/Terrain.hpp>
#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <Window/ImGuiSceneView.hpp>
#include <map>
#include <memory>
#include <string>
#include <variant>

using namespace std::string_literals;

class FileDialog {
  std::shared_ptr<imgui_addons::ImGuiFileBrowser> _fileDialog = nullptr;
  char* _textBuffer = nullptr;
  std::string _extensions = "";

 public:
  bool isVisible = false;

  FileDialog();
  ~FileDialog();

  void setup(char*, const std::string);
  void paint();
};

class ObjectAddFileDialog {
 private:
  using Attrib_t = std::variant<int, float, std::string>;

  inline static const std::string TITLE = "Add Object";

  std::shared_ptr<FileDialog> _fileDialog;
  std::string _objectTypes;
  std::shared_ptr<ViewerModel> _model;
  bool _isVisible = false;
  std::map<std::string, Attrib_t> _attributeBuffer;
  std::string _message = "";
  std::string _errorMessage = "";

 public:
  ObjectAddFileDialog(std::shared_ptr<ViewerModel>);
  ~ObjectAddFileDialog();

  void paint();
  void setVisible();
};
