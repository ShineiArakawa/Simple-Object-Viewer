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
  inline static const char* RENDER_TYPE_ITEMS = "Normal\0Color\0Texture\0Vertex Normal\0Shading\0Shading with texture\0Material\0";

  float _menuBarHeight;
  bool _isForcusedOnScene;
  int _renderTypeID;
  float _wheelOffset;
  ImVec2 _sceneAreaMin;
  ImVec2 _sceneAreaMax;
  ImGuiIO* _io;

  std::shared_ptr<ImGuiSceneView> _sceneView;
  std::shared_ptr<model::ViewerModel> _sceneModel;

  std::shared_ptr<ImGuiSceneView> _depthSceneView;
  std::shared_ptr<model::ViewerModel> _depthSceneModel;

  std::shared_ptr<ObjectAddFileDialog> _objectAddDialog;

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