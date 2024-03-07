# Project Name
project(SimView_Shader_object CXX)

add_library(
  ${PROJECT_NAME}
  OBJECT
  "DepthShader.cpp"
  "ModelShader.cpp"
  "Shader.cpp"
  "ShaderCompiler.cpp"
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