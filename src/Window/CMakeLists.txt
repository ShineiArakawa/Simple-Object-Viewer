project(SimView_Window_object CXX)

add_library(
  ${PROJECT_NAME}
  OBJECT
  "Window.cpp"
  "ImGuiSceneView.cpp"
  "ImGuiMainView.cpp"
  "FPSManager.cpp"
  "ImGuiObjectAddPanel.cpp"
)

# =========================================================
# Set Libraries ===========================================
# =========================================================
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  ${PROJECT_INCLUDE_DIR}
  ${EXTERNAL_INCLUDE_DIR}
)

if (SIMVIEW_WITH_VTK)
  target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    ${VTK_LIBRARIES}
  )
endif()
