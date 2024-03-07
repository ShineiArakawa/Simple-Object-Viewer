# Project Name
project(SimView_Renderer_object CXX)

add_library(
  ${PROJECT_NAME}
  OBJECT
  "Renderer.cpp"
  "DepthRenderer.cpp"
  "FrameBuffer.cpp"
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