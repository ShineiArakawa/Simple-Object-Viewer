#pragma once
#ifndef _APP_VIEWER_GUI_MAIN_H_
#define _APP_VIEWER_GUI_MAIN_H_

#include <App/common.hpp>
#include <ImGui.hpp>
#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <Util/FileUtil.hpp>
#include <Util/ModelParser.hpp>
#include <Window/ImGuiMainView.hpp>
#include <iostream>
#include <memory>

inline static int WINDOW_WIDTH = 1200;
inline static int WINDOW_HEIGHT = 800;
static const char* WINDOW_TITLE = "ViewerGUI";
static GLFWwindow* window = nullptr;
static std::shared_ptr<ViewerModel> model = nullptr;
static std::shared_ptr<ImGuiMainView> view = nullptr;
inline static const std::string DEFAULT_CONFIG_PATH = "../../../../data/sample_bunny.json";
#endif