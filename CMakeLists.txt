cmake_minimum_required(VERSION 3.0.0)
project(OpenGLExample CXX)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# include macros
FILE(GLOB _macro_files "${CMAKE_SOURCE_DIR}/cmake/macros/*.cmake")
FOREACH(_file ${_macro_files})
  MESSAGE(STATUS "Include ${_file}")
  INCLUDE(${_file})
ENDFOREACH()

# find path to modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")


include_directories(lib/glad/include)

add_subdirectory(
  src
)

add_subdirectory(
  lib
)
