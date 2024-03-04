project(OpenGLTemplate_Window_object CXX)

add_library(
  ${PROJECT_NAME}
  OBJECT
  "Window.cpp"
  "ImGuiSceneView.cpp"
  "ImGuiMainView.cpp"
  "ObjectAddPopup.cpp"
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