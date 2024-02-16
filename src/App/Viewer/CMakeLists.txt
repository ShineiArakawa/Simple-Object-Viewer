project(Viewer CXX)

add_executable(
  ${PROJECT_NAME}
  main.cpp
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
  ${GLEW_INCLUDE_DIR}
  ${GLM_INCLUDE_DIR}
  ${PICOJSON_INCLUDE_DIR}
  ${STB_INCLUDE_DIR}
  ${TINYOBJLOADER_INCLUDE_DIR}
  ${PROJECT_INCLUDE_DIR}
)


target_link_libraries(
  ${PROJECT_NAME}
  glad
  glfw
  $<TARGET_OBJECTS:Renderer_object>
  $<TARGET_OBJECTS:Model_object>
  $<TARGET_OBJECTS:Util_object>
  $<TARGET_OBJECTS:Window_object>
  ${CMAKE_DL_LIBS}
)

# add_custom_command(
# TARGET ${PROJECT_NAME} POST_BUILD
# COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>"/../Shaders"
# COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}"/../Shaders/" $<TARGET_FILE_DIR:${PROJECT_NAME}>"/../Shaders/"
# COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}"/../Shaders/" $<TARGET_FILE_DIR:${PROJECT_NAME}>"/../Shaders/"
# )
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_SOURCE_DIR}/bin/
)
