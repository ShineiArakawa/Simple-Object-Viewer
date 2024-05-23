cmake_minimum_required(VERSION 3.24)

project(SimpleObjectViewer C CXX)

############################################################################################################
# Compile Options 
############################################################################################################
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_POSITION_INDEPENDENT_CODE on)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
elseif(UNIX)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -Wall")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D SIMVIEW_ENABLE_LEAK_CHECK")

# NOTE: In some cases, failed to compile assimp with clang compiler with gcc 12 toolchain.
# Thus, specify your path to the gcc "11" toolchain by the following option.
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --gcc-install-dir=/usr/lib/gcc/x86_64-linux-gnu/11")

set(SIMVIEW_BUILD_AS_WIN32_APP TRUE CACHE BOOL "Build as WIN32 apps")
if (SIMVIEW_BUILD_AS_WIN32_APP)
  add_definitions(-DSIMVIEW_BUILD_AS_WIN32_APP)
endif()

############################################################################################################
# Envirionment Variables
############################################################################################################
# #### Set environment variables in project
set(PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(PROJECT_LIB_DIR "${PROJECT_SOURCE_DIR}/external")
set(WIN_RESOURCE_FILE "${PROJECT_SOURCE_DIR}/assets/resource.rc")

############################################################################################################
# External Libraries
############################################################################################################
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
set(SIMVIEW_WITH_ASSIMP TRUE CACHE BOOL "Build with Assimp")

if (SIMVIEW_WITH_ASSIMP)
  add_definitions(-DSIMVIEW_WITH_ASSIMP)
  set(ASSIMP_INCLUDE_DIR ${PROJECT_LIB_DIR}/assimp/include ${CMAKE_CURRENT_BINARY_DIR}/external/assimp/include)
  set(SIMVIEW_ASSIMP_LIBS assimp)
endif()


# #### VTK
set(SIMVIEW_WITH_VTK TRUE CACHE BOOL "Build with VTK")

if (SIMVIEW_WITH_VTK)
  find_package(VTK QUIET)

  if (VTK_FOUND)
    message(STATUS "### Found VTK")
    message(STATUS "### VTK_LIBRARIES : ${VTK_LIBRARIES}")
    add_definitions(-DSIMVIEW_WITH_VTK)
  else()
    set(SIMVIEW_WITH_VTK FALSE)
  endif()
endif()

############################################################################################################
# External libralies
############################################################################################################
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
  freetype
  ${SIMVIEW_ASSIMP_LIBS}
  ${VTK_LIBRARIES}
)

############################################################################################################
# For static library usage
############################################################################################################
# This variable is for the library mode
set(
  SimpleObjectViewer_INCLUDE_DIR
  ${PROJECT_INCLUDE_DIR}
  ${EXTERNAL_INCLUDE_DIR}
  CACHE
  STRING
  "All include directories for Simeple Object Viewer library"
)

set(SIMVIEW_BUILD_STATIC_LIBS TRUE CACHE BOOL "Build as static library")

set(SIMVIEW_COPY_BINARY_TO_BIN_DIR TRUE CACHE BOOL "Copy built banaries to bin/ directory")

set(SIMVIEW_BUILD_EXECUTABLES TRUE CACHE BOOL "Build executable files")

############################################################################################################
# Set install include directory
############################################################################################################
set(SIMVIEW_INSTALL TRUE CACHE BOOL "Install SimView library")

if (SIMVIEW_INSTALL)
  install(
  	DIRECTORY "${PROJECT_INCLUDE_DIR}/SimView"
  	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
	  FILES_MATCHING PATTERN "*.hpp"
  )
endif()

# NOTE: Installing lib directory is set in src/CMakeLists.txt

############################################################################################################
# Add subdirectory
############################################################################################################
add_subdirectory(src)
add_subdirectory(external)

############################################################################################################
# Message
############################################################################################################
message(STATUS "# =======================================================================================================")
message(STATUS "#  '${PROJECT_NAME}' build configuration ")
message(STATUS "# =======================================================================================================")
message(STATUS "#  [CMake]")
message(STATUS "#    CMake version                        : ${CMAKE_VERSION}")
message(STATUS "#    CMake build type                     : ${CMAKE_BUILD_TYPE}")
message(STATUS "#    CMAKE_CXX_FLAGS                      : ${CMAKE_CXX_FLAGS}")
message(STATUS "#    CMAKE_CXX_FLAGS_DEBUG                : ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "#    CMAKE_CXX_FLAGS_RELEASE              : ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "# ")
message(STATUS "#  [C/C++]")
message(STATUS "#    C   Compiler                         : ${CMAKE_C_COMPILER_ID} | ${CMAKE_C_COMPILER_VERSION} | ${CMAKE_C_COMPILER}")
message(STATUS "#    C++ Compiler                         : ${CMAKE_CXX_COMPILER_ID} | ${CMAKE_CXX_COMPILER_VERSION} | ${CMAKE_CXX_COMPILER}")
message(STATUS "#    CXX STANDARD                         : ${CMAKE_CXX_STANDARD}")
message(STATUS "# ")
message(STATUS "#  [OpenMP]")
message(STATUS "#    OpenMP_C_FLAGS                       : ${OpenMP_C_FLAGS}")
message(STATUS "#    OpenMP_CXX_FLAGS                     : ${OpenMP_CXX_FLAGS}")
message(STATUS "# ")
message(STATUS "#  [Environment Variables]")
message(STATUS "#    PROJECT_INCLUDE_DIR                  : ${PROJECT_INCLUDE_DIR}")
message(STATUS "#    PROJECT_LIB_DIR                      : ${PROJECT_LIB_DIR}")
message(STATUS "# ")
message(STATUS "#    GLAD_INCLUDE_DIR                     : ${GLAD_INCLUDE_DIR}")
message(STATUS "#    GLFW_INCLUDE_DIR                     : ${GLFW_INCLUDE_DIR}")
message(STATUS "#    GLM_INCLUDE_DIR                      : ${GLM_INCLUDE_DIR}")
message(STATUS "#    PICOJSON_INCLUDE_DIR                 : ${PICOJSON_INCLUDE_DIR}")
message(STATUS "#    STB_INCLUDE_DIR                      : ${STB_INCLUDE_DIR}")
message(STATUS "#    TINYOBJLOADER_INCLUDE_DIR            : ${TINYOBJLOADER_INCLUDE_DIR}")
message(STATUS "#    IMGUI_INCLUDE_DIR                    : ${IMGUI_INCLUDE_DIR}")
message(STATUS "#    IMGUI_SOURCE_DIR                     : ${IMGUI_SOURCE_DIR}")
message(STATUS "#    IMGUI_BACKENDS_DIR                   : ${IMGUI_BACKENDS_DIR}")
message(STATUS "#    NFD_INCLUDE_DIR                      : ${NFD_INCLUDE_DIR}")
message(STATUS "#    SPD_LOG_INCLUDE_DIR                  : ${SPD_LOG_INCLUDE_DIR}")
message(STATUS "#    ASSIMP_INCLUDE_DIR                   : ${ASSIMP_INCLUDE_DIR}")
message(STATUS "#    FREETYPE_INCLUDE_DIR                 : ${FREETYPE_INCLUDE_DIR}")
message(STATUS "# ")
message(STATUS "#    SIMVIEW_BUILD_EXECUTABLES            : ${SIMVIEW_BUILD_EXECUTABLES}")
message(STATUS "#    SIMVIEW_BUILD_STATIC_LIBS            : ${SIMVIEW_BUILD_STATIC_LIBS}")
message(STATUS "#    SIMVIEW_BUILD_AS_WIN32_APP           : ${SIMVIEW_BUILD_AS_WIN32_APP}")
message(STATUS "#    SIMVIEW_COPY_BINARY_TO_BIN_DIR       : ${SIMVIEW_COPY_BINARY_TO_BIN_DIR}")
message(STATUS "#    SIMVIEW_WITH_VTK                     : ${SIMVIEW_WITH_VTK}")
message(STATUS "# =======================================================================================================")
