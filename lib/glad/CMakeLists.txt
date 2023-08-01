cmake_minimum_required(VERSION 3.12)

project(glad)

add_library(
  ${PROJECT_NAME}
  STATIC
  "src/glad.c"
)
