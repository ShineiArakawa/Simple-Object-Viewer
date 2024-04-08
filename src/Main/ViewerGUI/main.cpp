#include <SimView/App/ViewerGUIApp.hpp>

#if defined(WIN32)
#include <Windows.h>

#if defined(ENABLE_LEAK_CHECK)
// Memory leak checker for MSVC
#include <crtdbg.h>
#endif  // ENABLE_LEAK_CHECK

#endif  // WIN32

#if defined(WIN32)
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow) {
  const int argc = __argc;
  char** argv = __argv;
#else
int main(int argc, char** argv) {
#endif
#if defined(WIN32) && defined(ENABLE_LEAK_CHECK)
  // Initialize memory leak checker for MSVC
  _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif  // ENABLE_LEAK_CHECK

  // Create application
  simview::app::ViewerGUIApp_t app = std::make_shared<simview::app::ViewerGUIApp>();

  // Set initial object if specified
  if (argc > 1) {
    std::string filePath(argv[1]);
    LOG_INFO("Initial object file: " + filePath);
    auto object = std::make_shared<simview::model::Object>(filePath);
    object->setName(simview::util::FileUtil::baseName(filePath));
    app->addObject(object);
  }

  // Run
  app->launch();

  return 0;
}
