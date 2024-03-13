#pragma once

#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <Window/ImGuiMainView.hpp>
#include <memory>
#include <string>

namespace simview {
namespace app {

class ViewerGUIApp {
 private:
  inline static int WINDOW_WIDTH = 2000;
  inline static int WINDOW_HEIGHT = 1200;
  inline static const char* WINDOW_TITLE = "ViewerGUI";
  inline static const std::string DEFAULT_CONFIG_PATH = "../../../../data/sample_bunny.json";

  GLFWwindow* _window = nullptr;
  model::ViewerModel_t _model = nullptr;
  window::ImGuiMainView_t _view = nullptr;

 public:
  ViewerGUIApp();
  ~ViewerGUIApp();

  void launch();
  void addObject(const model::Primitive_t& object, bool toInitializeVAO = true);
  void addBackground(const model::Background_t& background);
  void setRenderType(const model::Primitive::RenderType renderType);
  void setSideBarVisibility(const bool& isVisible);
};

using ViewerGUIApp_t = std::shared_ptr<ViewerGUIApp>;

}  // namespace app
}  // namespace simview