#include <App/ViewerGUIMain.hpp>

int main(int argc, char** argv) {
  // ====================================================================
  // Initialize GLFW window
  // ====================================================================
  if (glfwInit() == GLFW_FALSE) {
    fprintf(stderr, "Initialization failed!\n");
    return 1;
  }

  GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

  if (window == NULL) {
    glfwTerminate();
    fprintf(stderr, "Window creation failed!\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // ====================================================================
  // Initialize GLAD
  // ====================================================================
  if (glfwInit() == GLFW_FALSE) {
    fprintf(stderr, "Initialization failed!\n");
    return 1;
  }
  if (gladLoadGL() == 0) {
    fprintf(stderr, "Failed to load OpenGL 3.x/4.x libraries!\n");
    return 1;
  }

  // ====================================================================
  // Initialize Model
  // ====================================================================
  model = std::make_shared<ViewerModel>();
  model->setMaskMode(ImGuiSceneView::isMaskMode);

  // ====================================================================
  // Initialize UI
  // ====================================================================
  view = std::make_shared<ImGuiMainView>(window, model);

  // ====================================================================
  // Start main loop
  // ====================================================================
  while (!glfwWindowShouldClose(window) && view->moveOn) {
    view->paint();
    view->listenEvent();
    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
