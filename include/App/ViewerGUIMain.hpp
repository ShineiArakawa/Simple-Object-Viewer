#pragma once
#ifndef _APP_VIEWER_GUI_MAIN_H_
#define _APP_VIEWER_GUI_MAIN_H_

#include <App/ObjectAddPopup.hpp>
#include <App/common.hpp>
#include <ImGui.hpp>
#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <Util/FileUtil.hpp>
#include <Util/ModelParser.hpp>
#include <Window/ImGuiSceneView.hpp>
#include <iostream>
#include <memory>

inline static const int WINDOW_WIDTH = 1200;
inline static const int WINDOW_HEIGHT = 800;
static const char* WINDOW_TITLE = "ViewerGUI";
static GLFWwindow* window = nullptr;
static std::shared_ptr<ViewerModel> model = nullptr;
static std::shared_ptr<Renderer> renderer = nullptr;
static std::shared_ptr<ImGuiSceneView> scene = nullptr;
inline static const std::string DEFAULT_CONFIG_PATH = "../../../../data/sample_bunny.json";
inline static const int SETTING_TAB_HEIGHT = 300;
inline static const int SIDEBAR_WIDTH = 400;

#endif