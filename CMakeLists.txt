cmake_minimum_required(VERSION 3.24)
project(OpenGLExample CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE on)

#################################################
################ Compile Options ################
#################################################
if(MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "-Wall")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2")
elseif(UNIX)
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

#################################################
############# Environment Variables #############
#################################################
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(PROJECT_LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

#### Eigen
set(EIGEN_INCLUDE_DIR ${PROJECT_LIB_DIR}/eigen)

##### GLAD
set(GLAD_INCLUDE_DIR ${PROJECT_LIB_DIR}/glad/include)

#### GLFW
set(GLFW_INCLUDE_DIR ${PROJECT_LIB_DIR}/glfw/include)

##### GLM
set(GLM_INCLUDE_DIR ${PROJECT_LIB_DIR}/glm)

##### picojson
set(PICOJSON_INCLUDE_DIR ${PROJECT_LIB_DIR}/picojson)

##### stb
set(STB_INCLUDE_DIR ${PROJECT_LIB_DIR}/stb)

##### stb
set(TINYOBJLOADER_INCLUDE_DIR ${PROJECT_LIB_DIR}/tinyobjloader)
 
##### Dear Imgui
set(IMGUI_INCLUDE_DIR ${PROJECT_LIB_DIR}/imgui)
set(IMGUI_SOURCE_DIR ${PROJECT_LIB_DIR}/imgui)
set(IMGUI_BACKENDS_DIR ${IMGUI_SOURCE_DIR}/backends)
 
set(
  IMGUI_SOURCE_FILES
  ${IMGUI_SOURCE_DIR}/imgui.cpp
  ${IMGUI_SOURCE_DIR}/imgui_demo.cpp
  ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
  ${IMGUI_SOURCE_DIR}/imgui_tables.cpp
  ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
  ${IMGUI_SOURCE_DIR}/imgui_demo.cpp
  ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
  ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp
)

# #### Add subdirectory
add_subdirectory(pybind11)
add_subdirectory(src)
add_subdirectory(lib)
 
#################################################
#################### Message ####################
#################################################
message("# =======================================================================================================")
message("#  '${PROJECT_NAME}' build configuration ")
message("# =======================================================================================================")
message("#  [CMake]")
message("#    CMake version             : ${CMAKE_VERSION}")
message("#    CMake build type          : ${CMAKE_BUILD_TYPE}")
message("# ")
message("#  [C/C++]")
message("#    C++ Compiler              : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} ${CMAKE_CXX_COMPILER_WRAPPER} ${CMAKE_CXX_COMPILER}")
message("#    CXX STANDARD              : ${CMAKE_CXX_STANDARD}")
message("# ")
message("#  [Environment Variables]")
message("#    PROJECT_INCLUDE_DIR       : ${PROJECT_INCLUDE_DIR}")
message("#    PROJECT_LIB_DIR           : ${PROJECT_LIB_DIR}")
message("# ")
message("#    EIGEN_INCLUDE_DIR         : ${EIGEN_INCLUDE_DIR}")
message("#    GLAD_INCLUDE_DIR          : ${GLAD_INCLUDE_DIR}")
message("#    GLFW_INCLUDE_DIR          : ${GLFW_INCLUDE_DIR}")
message("#    GLM_INCLUDE_DIR           : ${GLM_INCLUDE_DIR}")
message("#    PICOJSON_INCLUDE_DIR      : ${PICOJSON_INCLUDE_DIR}")
message("#    STB_INCLUDE_DIR           : ${STB_INCLUDE_DIR}")
message("#    TINYOBJLOADER_INCLUDE_DIR : ${TINYOBJLOADER_INCLUDE_DIR}")
message("#    IMGUI_INCLUDE_DIR         : ${IMGUI_INCLUDE_DIR}")
message("#    IMGUI_SOURCE_DIR          : ${IMGUI_SOURCE_DIR}")
message("#    IMGUI_BACKENDS_DIR        : ${IMGUI_BACKENDS_DIR}")
message("# =======================================================================================================")





