# Project Name
project(SimView_Model_object CXX)

add_library(
  ${PROJECT_NAME}
  OBJECT
  "Box.cpp"
  "Object.cpp"
  "Background.cpp"
  "Terrain.cpp"
  "ViewerModel.cpp"
  "Model.cpp"
  "PoneModel.cpp"
  "Sphere.cpp"
  "PointCloud.cpp"
  "PointCloudPoly.cpp"
  "AxesCone.cpp"
  "MaterialObject.cpp"
  "AxisAlignedBoundingBox.cpp"
  "GridPlane.cpp"
  "WireFrame.cpp"
  "TextBox.cpp"
  "LightBall.cpp"
  "LineSet.cpp"
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

if (SIMVIEW_WITH_VTK)
  target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    ${VTK_LIBRARIES}
  )
endif()
