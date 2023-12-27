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
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/glad/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/glfw/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
