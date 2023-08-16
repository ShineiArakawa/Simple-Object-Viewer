project(Pone CXX)

add_executable(
  ${PROJECT_NAME}
  main.cpp
)


# =========================================================
# Set Libraries ===========================================
# =========================================================
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/glad/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/glfw/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/glm-0.9.9.8/glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/picojson)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)


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