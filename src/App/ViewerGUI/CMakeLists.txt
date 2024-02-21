project(ViewerGUI CXX)

add_executable(
  ${PROJECT_NAME}
  "main.cpp"
  ${IMGUI_SOURCE_FILES}
)

# =========================================================
# Set Libraries ===========================================
# =========================================================
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  ${EIGEN_INCLUDE_DIR}
  ${GLAD_INCLUDE_DIR}
  ${GLFW_INCLUDE_DIR}
  ${GLM_INCLUDE_DIR}
  ${PICOJSON_INCLUDE_DIR}
  ${STB_INCLUDE_DIR}
  ${TINYOBJLOADER_INCLUDE_DIR}
  ${IMGUI_INCLUDE_DIR}
  ${NFD_INCLUDE_DIR}
  ${SPD_LOG_INCLUDE_DIR}
  ${PROJECT_INCLUDE_DIR}
)


target_link_libraries(
  ${PROJECT_NAME}
  glad
  glfw
  nfd
  $<TARGET_OBJECTS:Renderer_object>
  $<TARGET_OBJECTS:Model_object>
  $<TARGET_OBJECTS:Util_object>
  $<TARGET_OBJECTS:Window_object>
  ${CMAKE_DL_LIBS}
)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_SOURCE_DIR}/bin/
)
