#pragma once

#define IDI_ICON1 101

#ifdef __cplusplus
#include <stdio.h>

#include <SimView/Model/ViewerModel.hpp>
#include <SimView/OpenGL.hpp>
#include <SimView/Window/ImGuiMainView.hpp>
#include <memory>
#include <string>

namespace simview {
namespace app {

class ViewerGUIApp {
 private:
  inline static int WINDOW_WIDTH = 2000;
  inline static int WINDOW_HEIGHT = 1200;
  inline static const char* WINDOW_TITLE = "Simple-Object-Viewer";

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
  void setWindowSubTitle(const std::string& subTitle);

  // Manual update

  bool shouldUpdateWindow() const;
  void paint() const;
};

using ViewerGUIApp_t = std::shared_ptr<ViewerGUIApp>;

}  // namespace app
}  // namespace simview
#endif
