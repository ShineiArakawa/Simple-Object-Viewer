#pragma once

#include <nfd.h>

#include <ImGui.hpp>
#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <Renderer/Renderer.hpp>
#include <Util/FontStorage.hpp>
#include <Window/FPSManager.hpp>
#include <Window/ImGuiObjectAddPanel.hpp>
#include <Window/ImGuiSceneView.hpp>
#include <iostream>
#include <memory>
#include <string>

namespace simview {
namespace window {

class ImGuiMainView {
 private:
  inline static const size_t CHAR_BUFFER_SIZE = 512;
  inline static const int SIDEBAR_WIDTH = 700;
  inline static const char* FLOAT_FORMAT = "%.6f";
  inline static const char* RENDER_TYPE_ITEMS = "Normal\0Color\0Texture\0Vertex Normal\0Shading\0Shading with texture\0Material\0";
  inline static const char* WIREFRAME_TYPE_ITEMS = "OFF\0ON\0Wire frame only\0";
  inline static const char* HELP_TEXT =
      "##### Simple Object Viewer #####\n"
      " \n"
      "MIT License\n"
      " \n"
      "Copyright (c) 2024 Shinei Arakawa\n"
      " \n"
      "Permission is hereby granted, free of charge, to any person obtaining a copy\n"
      "of this software and associated documentation files (the \"Software\"), to deal\n"
      "in the Software without restriction, including without limitation the rights\n"
      "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
      "copies of the Software, and to permit persons to whom the Software is\n"
      "furnished to do so, subject to the following conditions:\n"
      " \n"
      "The above copyright notice and this permission notice shall be included in all\n"
      "copies or substantial portions of the Software.\n"
      " \n"
      "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
      "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
      "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
      "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
      "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
      "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
      "SOFTWARE.\n"
      " \n"
      "[Comment]\n"
      "This program was created as part of learning 3D graphics using OpenGL. It starts\n"
      "with window creation using only GLFW and GLAD, then progressively includes\n"
      "additional features such as a GUI using Dear ImGui, the ability to load 3D shape files\n"
      "of various formats with Assimp, font rendering with FreeType, and so on.\n"
      "It has been developed with incremental feature additions. If you're interested\n"
      "in using these features but are unsure how, I hope this serves as a useful reference.";

  std::shared_ptr<float[]> _backgroundRGBABuffer = nullptr;
  std::shared_ptr<float[]> _wireFrameColorBuffer = nullptr;
  std::shared_ptr<float[]> _lightPositionBuffer = nullptr;
  std::shared_ptr<char[]> _screenshotFilePathBuffer = nullptr;

  bool _isVisibleSideBar;
  bool _isVisibleHelpMessage;

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

  std::shared_ptr<FPSManager> _fpsManager;

  bool _moveOn;

 public:
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

  bool shouldUpdate() const;

  bool toMoveOn() const;

  ImGuiSceneView_t getSceneView() const { return _sceneView; };

  model::ViewerModel_t getSceneModel() const { return _sceneModel; };

  ObjectAddFileDialog_t getObjectAddDialog() const { return _objectAddDialog; };

  void setRenderType(const model::Primitive::RenderType& renderType);

  void setSideBarVisibility(const bool& isVisible) { _isVisibleSideBar = isVisible; }
};

using ImGuiMainView_t = std::shared_ptr<ImGuiMainView>;

}  // namespace window
}  // namespace simview