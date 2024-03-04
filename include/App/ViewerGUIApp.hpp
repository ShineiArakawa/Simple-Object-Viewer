#pragma once

#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <Window/ImGuiMainView.hpp>
#include <memory>
#include <string>

// #define DEBUG_SHADOW_MAPPING

class ViewerGUIApp {
 private:
  inline static int WINDOW_WIDTH = 2000;
  inline static int WINDOW_HEIGHT = 1200;
  inline static const char* WINDOW_TITLE = "ViewerGUI";
  inline static const std::string DEFAULT_CONFIG_PATH = "../../../../data/sample_bunny.json";

  GLFWwindow* _window;
  std::shared_ptr<ViewerModel> _model;
  std::shared_ptr<ImGuiMainView> _view;

 public:
  ViewerGUIApp();
  ~ViewerGUIApp();

  void launch();
};
