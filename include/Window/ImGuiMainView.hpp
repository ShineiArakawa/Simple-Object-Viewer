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

namespace simview {
namespace window {

class ImGuiMainView {
 private:
  inline static const int SIDEBAR_WIDTH = 700;
  inline static const char* FLOAT_FORMAT = "%.6f";
  inline static const char* RENDER_TYPE_ITEMS = "Normal\0Color\0Texture\0Vertex Normal\0Shading\0Shading with texture\0Material\0";

  bool _isVisibleSideBar;

  float _menuBarHeight;
  bool _isForcusedOnScene;
  int _renderTypeID;
  float _wheelOffset;
  ImVec2 _sceneAreaMin;
  ImVec2 _sceneAreaMax;
  ImGuiIO* _io;

  ImGuiSceneView_t _sceneView;
  model::ViewerModel_t _sceneModel;

  ImGuiSceneView_t _depthSceneView;
  model::ViewerModel_t _depthSceneModel;

  ObjectAddFileDialog_t _objectAddDialog;

 public:
  bool moveOn;

 private:
  void paintMenuBar();
  void paintSideBar();
  void paintSceneWindow();
  void paintDepthSceneWindow();

 public:
  ImGuiMainView(GLFWwindow*, model::ViewerModel_t);
  ~ImGuiMainView();

  void paint();

  void listenEvent();

  void destroy();

  ImGuiSceneView_t getSceneView() const { return _sceneView; };

  model::ViewerModel_t getSceneModel() const { return _sceneModel; };

  ObjectAddFileDialog_t getObjectAddDialog() const { return _objectAddDialog; };

  void setRenderType(const model::Primitive::RenderType& renderType);

  void setSideBarVisibility(const bool& isVisible) { _isVisibleSideBar = isVisible; }
};

using ImGuiMainView_t = std::shared_ptr<ImGuiMainView>;

}  // namespace window
}  // namespace simview