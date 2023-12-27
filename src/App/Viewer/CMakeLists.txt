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
  ${CMAKE_SOURCE_DIR}/lib/eigen
  ${CMAKE_SOURCE_DIR}/lib/glad/include
  ${CMAKE_SOURCE_DIR}/lib/glfw/include
  ${CMAKE_SOURCE_DIR}/lib/glm
  ${CMAKE_SOURCE_DIR}/lib/picojson
  ${CMAKE_SOURCE_DIR}/lib/stb
  ${CMAKE_SOURCE_DIR}/lib/tinyobjloader
  ${CMAKE_SOURCE_DIR}/include
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
#   TARGET ${PROJECT_NAME} POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>"/../Shaders"
#   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}"/../Shaders/" $<TARGET_FILE_DIR:${PROJECT_NAME}>"/../Shaders/"
#   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}"/../Shaders/" $<TARGET_FILE_DIR:${PROJECT_NAME}>"/../Shaders/"
# )

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_SOURCE_DIR}/bin/
)
