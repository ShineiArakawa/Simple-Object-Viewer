cmake_minimum_required(VERSION 3.0.0)
project(OpenGLExample CXX)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
  add_compile_options("/O2")
else ()
  add_compile_options("-O3")
endif ()

include_directories(lib/glad/include)

add_subdirectory(
  src
)

add_subdirectory(
  lib
)
