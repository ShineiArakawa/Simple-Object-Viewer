# =========================================================
# Submodules ==============================================
# =========================================================

add_subdirectory(
    Main
)

add_subdirectory(
    App
)

add_subdirectory(
    Renderer
)

add_subdirectory(
    Model
)

add_subdirectory(
    Util
)

add_subdirectory(
    Window
)

add_subdirectory(
    Shader
)

# =========================================================
# Lib =====================================================
# =========================================================
# Project Name
project(libOpenGLTemplate CXX)

add_library(
  ${PROJECT_NAME}
  STATIC
  "App/PoneApp.cpp"
  "App/ViewerApp.cpp"
  "App/ViewerGUIApp.cpp"
  "Model/Box.cpp"
  "Model/AxisAlignedBoundingBox.cpp"
  "Model/Object.cpp"
  "Model/Background.cpp"
  "Model/Terrain.cpp"
  "Model/ViewerModel.cpp"
  "Model/Model.cpp"
  "Model/PoneModel.cpp"
  "Model/Sphere.cpp"
  "Model/PointCloud.cpp"
  "Model/PointCloudPoly.cpp"
  "Model/AxesCone.cpp"
  "Model/MaterialObject.cpp"
  "Renderer/Renderer.cpp"
  "Renderer/DepthRenderer.cpp"
  "Renderer/FrameBuffer.cpp"
  "Shader/DepthShader.cpp"
  "Shader/ModelShader.cpp"
  "Shader/Shader.cpp"
  "Shader/ShaderCompiler.cpp"
  "Util/Texture.cpp"
  "Util/ObjectLoader.cpp"
  "Util/ModelParser.cpp"
  "Util/FileUtil.cpp"
  "Util/StbAdapter.cpp"
  "Window/Window.cpp"
  "Window/ImGuiSceneView.cpp"
  "Window/ImGuiMainView.cpp"
  "Window/ObjectAddPopup.cpp"
  ${IMGUI_SOURCE_FILES}
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  ${OpenGLTemplate_INCLUDE_DIR}
)

target_link_libraries(
  ${PROJECT_NAME}
  ${EXTERNAL_LIBS}
  ${CMAKE_DL_LIBS}
)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_SOURCE_DIR}/bin/
)
