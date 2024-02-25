#pragma once
#ifndef _APP_VIEWER_GUI_MAIN_H_
#define _APP_VIEWER_GUI_MAIN_H_

#include <App/common.hpp>
#include <ImGui.hpp>
#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <Util/FileUtil.hpp>
#include <Util/Logging.hpp>
#include <Util/ModelParser.hpp>
#include <Window/ImGuiMainView.hpp>
#include <iostream>
#include <memory>

class ViewerGUIApp {
 private:
  inline static int WINDOW_WIDTH = 2000;
  inline static int WINDOW_HEIGHT = 1200;
  inline static const char* WINDOW_TITLE = "ViewerGUI";
  inline static GLFWwindow* window = nullptr;
  inline static std::shared_ptr<ViewerModel> model = nullptr;
  inline static std::shared_ptr<ImGuiMainView> view = nullptr;
  inline static const std::string DEFAULT_CONFIG_PATH = "../../../../data/sample_bunny.json";

 public:
  ViewerGUIApp() {
    Logging::setLevelFromEnv();

    // ====================================================================
    // Initialize GLFW window
    // ====================================================================
    if (glfwInit() == GLFW_FALSE) {
      LOG_CRITICAL("GLFW initialization failed!");
      exit(1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

    if (window == NULL) {
      glfwTerminate();
      LOG_CRITICAL("Window creation failed!");
      exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    LOG_INFO(glfwGetVersionString());

    // ====================================================================
    // Initialize GLAD
    // ====================================================================
    if (gladLoadGL() == GL_FALSE) {
      LOG_CRITICAL("Failed to load OpenGL 3.x/4.x libraries by glad!");
      exit(1);
    }

    // ====================================================================
    // Initialize Model
    // ====================================================================
    model = std::make_shared<ViewerModel>();
    model->compileShaders();

    // ====================================================================
    // Initialize UI
    // ====================================================================
    view = std::make_shared<ImGuiMainView>(window, model);
  }

  ~ViewerGUIApp() {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void launch() {
    // ====================================================================
    // Start main loop
    // ====================================================================
    while (!glfwWindowShouldClose(window) && view->moveOn) {
      view->paint();
      view->listenEvent();
      glfwSwapBuffers(window);
    }
  }
};

#endif