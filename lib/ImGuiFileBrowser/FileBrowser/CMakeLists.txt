cmake_minimum_required(VERSION 3.24)

project(imgui_filebrowser)

add_library(
  ${PROJECT_NAME}
  STATIC
  "ImGuiFileBrowser.cpp"
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  ${IMGUI_INCLUDE_DIR}
)