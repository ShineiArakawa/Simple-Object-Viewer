#include <App/ViewerGUIApp.hpp>

#if defined(WIN32) && defined(ENABLE_LEAK_CHECK)
#include <crtdbg.h>
#endif  // ENABLE_LEAK_CHECK

using namespace simview::app;

int main(int argc, char** argv) {
#if defined(WIN32) && defined(ENABLE_LEAK_CHECK)
  _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif  // ENABLE_LEAK_CHECK

  std::shared_ptr<ViewerGUIApp> app = std::make_shared<ViewerGUIApp>();

  app->launch();

  return 0;
}
