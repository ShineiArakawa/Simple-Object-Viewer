project(Pone CXX)

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
  $<TARGET_OBJECTS:SimView_App_object>
  $<TARGET_OBJECTS:SimView_Renderer_object>
  $<TARGET_OBJECTS:SimView_Model_object>
  $<TARGET_OBJECTS:SimView_Util_object>
  $<TARGET_OBJECTS:SimView_Window_object>
  $<TARGET_OBJECTS:SimView_Shader_object>
  ${CMAKE_DL_LIBS}
)
