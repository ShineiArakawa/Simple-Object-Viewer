cmake_minimum_required(VERSION 3.24)
project(OpenGLExample CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE on)

if(MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "-Wall")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2")
elseif(UNIX)
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

add_subdirectory(
  pybind11
)

add_subdirectory(
  src
)

add_subdirectory(
  lib
)
