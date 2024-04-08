#pragma once

#include <SimView/Model/PoneModel.hpp>
#include <SimView/Util/ModelParser.hpp>
#include <SimView/Window/Window.hpp>
#include <memory>
#include <string>

namespace simview {
namespace app {

class PoneApp {
 public:
  inline static const char* WIN_TITLE = "Pone";
  inline static model::PoneModel_t model = nullptr;
  inline static GLFWwindow* window = nullptr;
  inline static const std::string DEFAULT_CONFIG_PATH = "../data/sample.json";

 public:
  PoneApp(const std::string& configFilePath);
  ~PoneApp();
  void launch();
  static void keyboardEventPone(GLFWwindow* window, int key, int scancode, int action, int mods);
};

using PoneApp_t = std::shared_ptr<PoneApp>;

}  // namespace app
}  // namespace simview
