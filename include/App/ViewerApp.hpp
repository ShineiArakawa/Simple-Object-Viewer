#pragma once

#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <Util/ModelParser.hpp>
#include <Window/Window.hpp>
#include <memory>
#include <string>

namespace simview {
namespace app {
class ViewerApp {
 public:
  inline static const char* WIN_TITLE = "Viewer";
  inline static const std::string DEFAULT_CONFIG_PATH = "../data/sample_bunny.json";
  inline static std::shared_ptr<model::ViewerModel> model = nullptr;
  inline static GLFWwindow* window = nullptr;

 public:
  ViewerApp(std::string configFilePath);
  ~ViewerApp();

  void launch();

  static void keyboardEventViewer(GLFWwindow* window, int key, int scancode, int action, int mods);
};
}  // namespace app
}  // namespace simview