project(Viewer CXX)

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
  ${PROJECT_INCLUDE_DIR}
  ${EXTERNAL_INCLUDE_DIR}
)


target_link_libraries(
  ${PROJECT_NAME}
  ${EXTERNAL_LIBS}
  $<TARGET_OBJECTS:OpenGLTemplate_App_object>
  $<TARGET_OBJECTS:OpenGLTemplate_Renderer_object>
  $<TARGET_OBJECTS:OpenGLTemplate_Model_object>
  $<TARGET_OBJECTS:OpenGLTemplate_Util_object>
  $<TARGET_OBJECTS:OpenGLTemplate_Window_object>
  $<TARGET_OBJECTS:OpenGLTemplate_Shader_object>
  ${CMAKE_DL_LIBS}
)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_SOURCE_DIR}/bin/
)