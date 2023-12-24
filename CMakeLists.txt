cmake_minimum_required(VERSION 3.24)
project(OpenGLExample CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE on)

# add_compile_options("-Wall")
if(MSVC)
  add_compile_options("/O2")
else()
  add_compile_options("-O3")
endif()

include_directories(lib/glad/include)

add_subdirectory(
  pybind11
)

add_subdirectory(
  src
)

add_subdirectory(
  lib
)
