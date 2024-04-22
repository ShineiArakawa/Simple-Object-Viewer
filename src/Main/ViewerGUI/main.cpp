#include <SimView/App/ViewerGUIApp.hpp>

#if defined(_WIN32)

#include <Windows.h>

#if defined(SIMVIEW_ENABLE_LEAK_CHECK)
#include <crtdbg.h>  // Memory leak checker for MSVC
#endif               // SIMVIEW_ENABLE_LEAK_CHECK

#endif  // _WIN32

#if defined(_WIN32) && defined(SIMVIEW_BUILD_AS_WIN32_APP)
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow) {
  const int argc = __argc;
  char** argv = __argv;
#else   // _WIN32 && SIMVIEW_BUILD_AS_WIN32_APP
int main(int argc, char** argv) {
#endif  // _WIN32 && SIMVIEW_BUILD_AS_WIN32_APP

#if defined(_WIN32) && defined(SIMVIEW_ENABLE_LEAK_CHECK)
  // Initialize memory leak checker for MSVC
  _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif  // _WIN32 && SIMVIEW_ENABLE_LEAK_CHECK

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