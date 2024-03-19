#include <App/ViewerGUIApp.hpp>

#if defined(WIN32)
#include <Windows.h>

#if defined(ENABLE_LEAK_CHECK)
// Memory leak checker for MSVC
#include <crtdbg.h>
#endif  // ENABLE_LEAK_CHECK

#endif  // WIN32

using namespace simview::app;

// #if defined(WIN32)
// int WINAPI WinMain(
//     HINSTANCE hInstance,
//     HINSTANCE hPrevInstance,
//     LPSTR lpCmdLine,
//     int nCmdShow) {
// #else
// int main(int argc, char** argv) {
// #endif
int main(int argc, char** argv) {
#if defined(WIN32) && defined(ENABLE_LEAK_CHECK)
  // Initialize memory leak checker for MSVC
  _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif  // ENABLE_LEAK_CHECK

  ViewerGUIApp_t app = std::make_shared<ViewerGUIApp>();

  app->launch();

  return 0;
}
