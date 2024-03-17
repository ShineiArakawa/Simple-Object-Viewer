cmake_minimum_required(VERSION 3.24)

project(SimpleObjectViewer C CXX)

# #################################################
# #### Compile Options ############################
# #################################################
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_POSITION_INDEPENDENT_CODE on)

if(MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
elseif(UNIX)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D ENABLE_LEAK_CHECK")

# NOTE: In some cases, failed to compile assimp with clang compiler with gcc 12 toolchain.
# Thus, specify your path to the gcc "11" toolchain by the following option.
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --gcc-install-dir=/usr/lib/gcc/x86_64-linux-gnu/11")

# #################################################
# #### Envirionment Variables #####################
# #################################################
# #### Set environment variables in project
set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_LIB_DIR ${PROJECT_SOURCE_DIR}/external)

# #################################################
# #### External Libraries #########################
# #################################################
# #### OpenMP
find_package(OpenMP REQUIRED)

if(OpenMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# #### GLAD
set(GLAD_INCLUDE_DIR ${PROJECT_LIB_DIR}/glad/include)

# #### GLFW
set(GLFW_INCLUDE_DIR ${PROJECT_LIB_DIR}/glfw/include)

# #### GLM
set(GLM_INCLUDE_DIR ${PROJECT_LIB_DIR}/glm)

# #### picojson
set(PICOJSON_INCLUDE_DIR ${PROJECT_LIB_DIR}/picojson)

# #### stb
set(STB_INCLUDE_DIR ${PROJECT_LIB_DIR}/stb)

# #### tinyobjloader
set(TINYOBJLOADER_INCLUDE_DIR ${PROJECT_LIB_DIR}/tinyobjloader)

# #### freetype
set(FREETYPE_INCLUDE_DIR ${PROJECT_LIB_DIR}/freetype/include)

# #### Dear Imgui
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
  ${IMGUI_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
)

# #### nativefiledialog-extended
set(NFD_INCLUDE_DIR ${PROJECT_LIB_DIR}/nativefiledialog-extended/src/include)

# #### spdlog
set(SPD_LOG_INCLUDE_DIR ${PROJECT_LIB_DIR}/spdlog/include)

# #### assimp
set(ASSIMP_INCLUDE_DIR ${PROJECT_LIB_DIR}/assimp/include ${CMAKE_CURRENT_BINARY_DIR}/external/assimp/include)

######################################################
# External libralies
######################################################
set(
  EXTERNAL_INCLUDE_DIR
  ${GLAD_INCLUDE_DIR}
  ${GLFW_INCLUDE_DIR}
  ${GLM_INCLUDE_DIR}
  ${PICOJSON_INCLUDE_DIR}
  ${STB_INCLUDE_DIR}
  ${TINYOBJLOADER_INCLUDE_DIR}
  ${IMGUI_INCLUDE_DIR}
  ${NFD_INCLUDE_DIR}
  ${SPD_LOG_INCLUDE_DIR}
  ${ASSIMP_INCLUDE_DIR}
  ${FREETYPE_INCLUDE_DIR}
)

set(
  EXTERNAL_LIBS
  glad
  glfw
  nfd
  assimp
  freetype
)

######################################################
# For static library usage
######################################################
# This variable is for the library mode
set(
  SimpleObjectViewer_INCLUDE_DIR
  ${PROJECT_INCLUDE_DIR}
  ${EXTERNAL_INCLUDE_DIR}
  CACHE
  STRING
  "All include directories for Simeple Object Viewer library"
)

# Set install include directory
install(
	DIRECTORY "${CMAKE_SOURCE_DIR}/include/" 
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/SimView/"
	FILES_MATCHING PATTERN "*.hpp"
)

set(BUILD_SIMVIEW_STATIC_LIBS TRUE CACHE BOOL "" FORCE)

# Set install lib directory is done in erc/CMakeLists.txt

######################################################
# Add subdirectory
######################################################
add_subdirectory(src)
add_subdirectory(external)

# #################################################
# #### Message ####################################
# #################################################
message("# =======================================================================================================")
message("#  '${PROJECT_NAME}' build configuration ")
message("# =======================================================================================================")
message("#  [CMake]")
message("#    CMake version                        : ${CMAKE_VERSION}")
message("#    CMake build type                     : ${CMAKE_BUILD_TYPE}")
message("#    CMAKE_CXX_FLAGS                      : ${CMAKE_CXX_FLAGS}")
message("#    CMAKE_CXX_FLAGS_DEBUG                : ${CMAKE_CXX_FLAGS_DEBUG}")
message("#    CMAKE_CXX_FLAGS_RELEASE              : ${CMAKE_CXX_FLAGS_RELEASE}")
message("# ")
message("#  [C/C++]")
message("#    C   Compiler                         : ${CMAKE_C_COMPILER_ID} | ${CMAKE_C_COMPILER_VERSION} | ${CMAKE_C_COMPILER}")
message("#    C++ Compiler                         : ${CMAKE_CXX_COMPILER_ID} | ${CMAKE_CXX_COMPILER_VERSION} | ${CMAKE_CXX_COMPILER}")
message("#    CXX STANDARD                         : ${CMAKE_CXX_STANDARD}")
message("# ")
message("#  [OpenMP]")
message("#    OpenMP_C_FLAGS                       : ${OpenMP_C_FLAGS}")
message("#    OpenMP_CXX_FLAGS                     : ${OpenMP_CXX_FLAGS}")
message("# ")
message("#  [Environment Variables]")
message("#    PROJECT_INCLUDE_DIR                  : ${PROJECT_INCLUDE_DIR}")
message("#    PROJECT_LIB_DIR                      : ${PROJECT_LIB_DIR}")
message("# ")
message("#    GLAD_INCLUDE_DIR                     : ${GLAD_INCLUDE_DIR}")
message("#    GLFW_INCLUDE_DIR                     : ${GLFW_INCLUDE_DIR}")
message("#    GLM_INCLUDE_DIR                      : ${GLM_INCLUDE_DIR}")
message("#    PICOJSON_INCLUDE_DIR                 : ${PICOJSON_INCLUDE_DIR}")
message("#    STB_INCLUDE_DIR                      : ${STB_INCLUDE_DIR}")
message("#    TINYOBJLOADER_INCLUDE_DIR            : ${TINYOBJLOADER_INCLUDE_DIR}")
message("#    IMGUI_INCLUDE_DIR                    : ${IMGUI_INCLUDE_DIR}")
message("#    IMGUI_SOURCE_DIR                     : ${IMGUI_SOURCE_DIR}")
message("#    IMGUI_BACKENDS_DIR                   : ${IMGUI_BACKENDS_DIR}")
message("#    NFD_INCLUDE_DIR                      : ${NFD_INCLUDE_DIR}")
message("#    SPD_LOG_INCLUDE_DIR                  : ${SPD_LOG_INCLUDE_DIR}")
message("#    ASSIMP_INCLUDE_DIR                   : ${ASSIMP_INCLUDE_DIR}")
message("#    FREETYPE_INCLUDE_DIR                 : ${FREETYPE_INCLUDE_DIR}")
message("# ")
message("#    BUILD_SIMVIEW_STATIC_LIBS            : ${BUILD_SIMVIEW_STATIC_LIBS}")
message("# =======================================================================================================")



