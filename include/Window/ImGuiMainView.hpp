#pragma once

#include <nfd.h>

#include <ImGui.hpp>
#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <Renderer/Renderer.hpp>
#include <Window/ImGuiSceneView.hpp>
#include <Window/ObjectAddPopup.hpp>
#include <iostream>
#include <memory>
#include <string>

namespace oglt {
namespace window {
class ImGuiMainView {
 private:
  inline static const int SIDEBAR_WIDTH = 700;
  inline static const char* FLOAT_FORMAT = "%.6f";

  float _menuBarHeight = -1.0f;
  bool _isForcusedOnScene = false;
  float _wheelOffset = 0.0f;
  ImVec2 _sceneAreaMin = ImVec2(0.0f, 0.0f);
  ImVec2 _sceneAreaMax = ImVec2(0.0f, 0.0f);
  ImGuiIO* _io;

  std::shared_ptr<ImGuiSceneView> _sceneView = nullptr;
  std::shared_ptr<model::ViewerModel> _sceneModel = nullptr;

  std::shared_ptr<ImGuiSceneView> _depthSceneView = nullptr;
  std::shared_ptr<model::ViewerModel> _depthSceneModel = nullptr;

  std::shared_ptr<ObjectAddFileDialog> _objectAddDialog = nullptr;

 public:
  bool moveOn = true;

 private:
  void paintMenuBar();
  void paintSideBar();
  void paintSceneWindow();
  void paintDepthSceneWindow();

 public:
  ImGuiMainView(GLFWwindow*, std::shared_ptr<model::ViewerModel>);
  ~ImGuiMainView();

  void paint();
  void listenEvent();

  void destroy();

  std::shared_ptr<ImGuiSceneView> getSceneView() { return _sceneView; };
  std::shared_ptr<model::ViewerModel> getSceneModel() { return _sceneModel; };
  std::shared_ptr<ObjectAddFileDialog> getObjectAddDialog() { return _objectAddDialog; };
};
}  // namespace window
}  // namespace oglt