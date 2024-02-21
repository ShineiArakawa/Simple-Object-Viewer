#include <App/ViewerGUIMain.hpp>

int main(int argc, char** argv) {
  std::shared_ptr<ViewerGUIApp> app = std::make_shared<ViewerGUIApp>();

  app->launch();

  return 0;
}
