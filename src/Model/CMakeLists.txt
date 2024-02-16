# Project Name
project(Model_object CXX)

add_library(
  ${PROJECT_NAME}
  OBJECT
  "Box.cpp"
  "ShaderCompiler.cpp"
  "Object.cpp"
  "Background.cpp"
  "Terrain.cpp"
  "ViewerModel.cpp"
  "Model.cpp"
  "PoneModel.cpp"
)

# =========================================================
# Set Libraries ===========================================
# =========================================================
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  ${EIGEN_INCLUDE_DIR}
  ${GLAD_INCLUDE_DIR}
  ${GLFW_INCLUDE_DIR}
  ${GLM_INCLUDE_DIR}
  ${PICOJSON_INCLUDE_DIR}
  ${STB_INCLUDE_DIR}
  ${TINYOBJLOADER_INCLUDE_DIR}
  ${PROJECT_INCLUDE_DIR}
)