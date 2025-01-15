project(Simple-Object-Viewer CXX)

set(
  VIEWER_GUI_SOURCES
  "main.cpp"
  ${IMGUI_SOURCE_FILES}
)

if(WIN32 AND SIMVIEW_BUILD_AS_WIN32_APP)
  list(APPEND VIEWER_GUI_SOURCES ${WIN_RESOURCE_FILE})
  add_executable(
    ${PROJECT_NAME}
    WIN32
    ${VIEWER_GUI_SOURCES}
  )
elseif (APPLE)
  add_executable(
    ${PROJECT_NAME}
    MACOSX_BUNDLE
    ${VIEWER_GUI_SOURCES}
  )
else ()
  add_executable(
    ${PROJECT_NAME}
    ${VIEWER_GUI_SOURCES}
  )
endif()

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

############################################################################################################
# Install
############################################################################################################
install(
  TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION "${CMAKE_INSTALL_BINDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  BUNDLE DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
