#pragma once
#ifndef _APP_VIEWER_GUI_MAIN_H_
#define _APP_VIEWER_GUI_MAIN_H_

#include <App/common.hpp>
#include <ImGui.hpp>
#include <Window/ImGuiWindow.hpp>

inline static const int WINDOW_WIDTH = 1200;
inline static const int WINDOW_HEIGHT = 800;
static const char* WIN_TITLE = "ViewerGUI";
static pViewerModel model = nullptr;
static const std::string DEFAULT_CONFIG_PATH = "../../../../data/sample_bunny.json";
inline static const int SETTING_TAB_HEIGHT = 300;
inline static const int SIDEBAR_WIDTH = 400;

#endif