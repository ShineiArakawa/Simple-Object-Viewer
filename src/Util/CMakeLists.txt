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
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/glad/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/glfw/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/stb)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/tinyobjloader)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/picojson)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
