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
  ${CMAKE_SOURCE_DIR}/lib/eigen
  ${CMAKE_SOURCE_DIR}/lib/glad/include
  ${CMAKE_SOURCE_DIR}/lib/glfw/include
  ${CMAKE_SOURCE_DIR}/lib/glm
  ${CMAKE_SOURCE_DIR}/lib/picojson
  ${CMAKE_SOURCE_DIR}/lib/stb
  ${CMAKE_SOURCE_DIR}/lib/tinyobjloader
  ${CMAKE_SOURCE_DIR}/include
)