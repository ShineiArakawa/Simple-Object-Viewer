#pragma once

#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <Window/ImGuiMainView.hpp>
#include <memory>
#include <string>

// #define DEBUG_SHADOW_MAPPING

namespace simview {
namespace app {
class ViewerGUIApp {
 private:
  inline static int WINDOW_WIDTH = 2000;
  inline static int WINDOW_HEIGHT = 1200;
  inline static const char* WINDOW_TITLE = "ViewerGUI";
  inline static const std::string DEFAULT_CONFIG_PATH = "../../../../data/sample_bunny.json";

  GLFWwindow* _window;
  std::shared_ptr<model::ViewerModel> _model;
  std::shared_ptr<window::ImGuiMainView> _view;

 public:
  ViewerGUIApp();
  ~ViewerGUIApp();

  void launch();
  void addObject(const model::Model::t_object& object, bool toInitializeVAO = true);
  void addBackground(const model::Model::t_background& background);
  void setRenderType(const model::Primitives::RenderType renderType);
};
}  // namespace app
}  // namespace simview