#pragma once
#ifndef _APP_VIEWER_MAIN_H_
#define _APP_VIEWER_MAIN_H_

#include <App/common.hpp>

static const char* WIN_TITLE = "Viewer";
static pViewerModel model = nullptr;

void keyboardEventViewer(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif