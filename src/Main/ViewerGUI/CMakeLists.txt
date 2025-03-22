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

  # ${MACOS_ICON_FILE} is exist?
  if(EXISTS ${MACOS_ICON_FILE})
    # Add icon file to the bundle
    set_source_files_properties(
      ${MACOS_ICON_FILE}
      PROPERTIES
      MACOSX_PACKAGE_LOCATION "Resources"
    )

    target_sources(
      ${PROJECT_NAME}
      PRIVATE
      ${MACOS_ICON_FILE}
    )
  else()
    message(WARNING "Icon file not found: ${MACOS_ICON_FILE}. The built application will not have an icon. If you need an icon, run 'make_icon_set.sh' in the 'assets' directory beforehand.")
  endif()
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
