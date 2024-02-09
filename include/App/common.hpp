#pragma once
#ifndef _APP_COMMON_MAIN_H_
#define _APP_COMMON_MAIN_H_

#include <Model/Background.hpp>
#include <Model/Box.hpp>
#include <Model/Model.hpp>
#include <Model/Object.hpp>
#include <Model/PoneModel.hpp>
#include <Model/Primitives.hpp>
#include <Model/ShaderCompiler.hpp>
#include <Model/Terrain.hpp>
#include <Model/ViewerModel.hpp>
#include <OpenGL.hpp>
#include <Renderer/Renderer.hpp>
#include <Util/ModelParser.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

typedef std::shared_ptr<Model> pModel;
typedef std::shared_ptr<ViewerModel> pViewerModel;
typedef std::shared_ptr<PoneModel> pPoneModel;

#endif  // _COMMON_H_