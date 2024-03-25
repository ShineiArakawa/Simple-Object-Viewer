#pragma once

// ImGui
#include "ImGui.hpp"

// OpenGL
#include "OpenGL.hpp"

// App
#include "App/PoneApp.hpp"
#include "App/ViewerApp.hpp"
#include "App/ViewerGUIApp.hpp"

// Model
#include "Model/AxesCone.hpp"
#include "Model/AxisAlignedBoundingBox.hpp"
#include "Model/Background.hpp"
#include "Model/Box.hpp"
#include "Model/MaterialObject.hpp"
#include "Model/Model.hpp"
#include "Model/Object.hpp"
#include "Model/PointCloud.hpp"
#include "Model/PointCloudPoly.hpp"
#include "Model/PoneModel.hpp"
#include "Model/Primitives.hpp"
#include "Model/Sphere.hpp"
#include "Model/Terrain.hpp"
#include "Model/TextBox.hpp"
#include "Model/ViewerModel.hpp"

// Renderer
#include "Renderer/DepthRenderer.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/Renderer.hpp"

// Shader
#include "Shader/DefaultShaders.hpp"
#include "Shader/DepthShader.hpp"
#include "Shader/ModelShader.hpp"
#include "Shader/Shader.hpp"
#include "Shader/ShaderCompiler.hpp"

// Utility
#include "Util/DataStructure.hpp"
#include "Util/FileUtil.hpp"
#include "Util/FontStorage.hpp"
#include "Util/Geometry.hpp"
#include "Util/Logging.hpp"
#include "Util/Math.hpp"
#include "Util/ModelParser.hpp"
#include "Util/ObjectLoader.hpp"
#include "Util/StbAdapter.hpp"
#include "Util/Texture.hpp"
#include "Util/StreamExecutor.hpp"
#include "Util/Colors.hpp"

// Window
#include "Window/FPSManager.hpp"
#include "Window/ImGuiMainView.hpp"
#include "Window/ImGuiObjectAddPanel.hpp"
#include "Window/ImGuiSceneView.hpp"
#include "Window/Window.hpp"

namespace simview {

inline static const char* VERSION = "v0.0";

}
