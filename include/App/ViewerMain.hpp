#pragma once
#ifndef _APP_VIEWER_MAIN_H_
#define _APP_VIEWER_MAIN_H_

#include <App/common.hpp>
#include <Window/Window.hpp>

static const char* WIN_TITLE = "Viewer";
static pViewerModel model = nullptr;
static const std::string DEFAULT_CONFIG_PATH = "../data/sample_bunny.json";

#endif