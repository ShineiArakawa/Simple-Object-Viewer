#include <App/ViewerApp.hpp>

#if defined(WIN32) && defined(ENABLE_LEAK_CHECK)
#include <crtdbg.h>
#endif  // ENABLE_LEAK_CHECK

using namespace oglt::app;
using namespace oglt::util;

int main(int argc, char** argv) {
#if defined(WIN32) && defined(ENABLE_LEAK_CHECK)
  _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif  // ENABLE_LEAK_CHECK

  // Parse args
  int nArgs = argc - 1;
  std::cout << "Number of arguments : " + std::to_string(nArgs) << std::endl;
  for (int iArg = 0; iArg < nArgs; iArg++) {
    std::cout << "args[" << iArg << "]=" << argv[iArg + 1] << std::endl;
  }

  // Parse config file path
  std::string configFilePath;
  if (nArgs > 0) {
    configFilePath = argv[1];
  } else {
    std::cout << "The config file path was not specified. Continue with the default config path: " << FileUtil::absPath(ViewerApp::DEFAULT_CONFIG_PATH)
              << std::endl;
    configFilePath = ViewerApp::DEFAULT_CONFIG_PATH;
  }

  // Check config file
  if (!FileUtil::exists(configFilePath)) {
    std::cerr << "Failed to open the config file. Please check the arguments." << std::endl;
    std::cerr << "args: {config_file}" << std::endl;
    std::exit(1);
  }

  std::shared_ptr<ViewerApp> app = std::make_shared<ViewerApp>(configFilePath);

  app->launch();

  return 0;
}
