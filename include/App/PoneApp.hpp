#pragma once

#include <Model/PoneModel.hpp>
#include <Util/ModelParser.hpp>
#include <Window/Window.hpp>
#include <memory>
#include <string>

namespace oglt {
namespace app {
class PoneApp {
 public:
  inline static const char* WIN_TITLE = "Pone";
  inline static std::shared_ptr<model::PoneModel> model = nullptr;
  inline static GLFWwindow* window = nullptr;
  inline static const std::string DEFAULT_CONFIG_PATH = "../data/sample.json";

 public:
  PoneApp(const std::string& configFilePath);
  ~PoneApp();
  void launch();
  static void keyboardEventPone(GLFWwindow* window, int key, int scancode, int action, int mods);
};

}  // namespace app
}  // namespace oglt
