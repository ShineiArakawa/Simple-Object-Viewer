# =========================================================
# Submodules ==============================================
# =========================================================
if (SIMVIEW_BUILD_EXECUTABLES)
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
endif()

# =========================================================
# Lib =====================================================
# =========================================================
if (SIMVIEW_BUILD_STATIC_LIBS)
    message(STATUS "Add rule to build Simple Object Viewer as a static library.")

    # Add build rule
    project(SimView_static CXX)

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
      "Model/WireFrame.cpp"
      "Model/GridPlane.cpp"
      "Model/TextBox.cpp"
      "Model/LightBall.cpp"
      "Model/LineSet.cpp"
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
      "Util/Geometry.cpp"
      "Util/FontStorage.cpp"
      "Util/StreamExecutor.cpp"
      "Util/Colors.cpp"
      "Window/Window.cpp"
      "Window/ImGuiSceneView.cpp"
      "Window/ImGuiMainView.cpp"
      "Window/ImGuiObjectAddPanel.cpp"
      "Window/FPSManager.cpp"
      ${IMGUI_SOURCE_FILES}
      ${WIN_RESOURCE_FILE}
    )

    target_include_directories(
      ${PROJECT_NAME}
      PUBLIC
      ${SimpleObjectViewer_INCLUDE_DIR}
    )

    target_link_libraries(
      ${PROJECT_NAME}
      ${EXTERNAL_LIBS}
      ${CMAKE_DL_LIBS}
    )

    if (SIMVIEW_INSTALL)
        install(
            TARGETS ${PROJECT_NAME}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )
    endif()
endif()
