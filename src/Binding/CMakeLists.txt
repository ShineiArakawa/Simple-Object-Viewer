project(Binding CXX)

set(BINDING_MODULE_NAME ViewerPythonBinding)

set(CMAKE_POSITION_INDEPENDENT_CODE on)

pybind11_add_module(
  ${BINDING_MODULE_NAME}
  main.cpp
  $<TARGET_OBJECTS:Renderer_object>
  $<TARGET_OBJECTS:Model_object>
  $<TARGET_OBJECTS:Util_object>
  $<TARGET_OBJECTS:Window_object>
)

# =========================================================
# Set Libraries ===========================================
# =========================================================
target_include_directories(
  ${BINDING_MODULE_NAME}
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


# target_link_libraries(
# ${BINDING_MODULE_NAME}
# PRIVATE
# glad
# glfw
# $<TARGET_OBJECTS:Renderer_object>
# $<TARGET_OBJECTS:Model_object>
# $<TARGET_OBJECTS:Util_object>
# $<TARGET_OBJECTS:Window_object>
# ${CMAKE_DL_LIBS}
# )
target_link_libraries(
  ${BINDING_MODULE_NAME}
  PRIVATE
  glad
  glfw
  ${CMAKE_DL_LIBS}
)

add_custom_command(
  TARGET ${BINDING_MODULE_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${BINDING_MODULE_NAME}> ${CMAKE_SOURCE_DIR}/python/ViewerPythonBinding/lib/
)
