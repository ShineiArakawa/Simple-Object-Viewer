# Project Name
project(Util_object CXX)

add_library(
    ${PROJECT_NAME}
    OBJECT
    "Texture.cpp"
    "ObjectLoader.cpp"
    "ModelParser.cpp"
    "FileUtil.cpp"
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