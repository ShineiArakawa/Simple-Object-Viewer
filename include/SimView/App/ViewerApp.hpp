#pragma once

#include <SimView/Model/ViewerModel.hpp>
#include <SimView/OpenGL.hpp>
#include <SimView/Util/ModelParser.hpp>
#include <SimView/Window/Window.hpp>
#include <memory>
#include <string>

namespace simview {
namespace app {

class ViewerApp {
 public:
  inline static const char* WIN_TITLE = "Viewer";
  inline static const std::string DEFAULT_CONFIG_PATH = "../data/sample_bunny.json";
  inline static model::ViewerModel_t model = nullptr;
  inline static GLFWwindow* window = nullptr;

 public:
  ViewerApp(std::string configFilePath);
  ~ViewerApp();

  void launch();

  static void keyboardEventViewer(GLFWwindow* window, int key, int scancode, int action, int mods);
};

using ViewerApp_t = std::shared_ptr<ViewerApp>;

}  // namespace app
}  // namespace simview