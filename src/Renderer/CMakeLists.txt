# Project Name
project(Renderer_object CXX)

add_library(
  ${PROJECT_NAME}
  OBJECT
  "Renderer.cpp"
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