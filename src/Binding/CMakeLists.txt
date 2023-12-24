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
target_include_directories(${BINDING_MODULE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/glad/include)
target_include_directories(${BINDING_MODULE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/glfw/include)
target_include_directories(${BINDING_MODULE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/glm-0.9.9.8/glm)
target_include_directories(${BINDING_MODULE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/picojson)
target_include_directories(${BINDING_MODULE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

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
