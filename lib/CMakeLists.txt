add_subdirectory(
    eigen
)

add_subdirectory(
    glad
)

add_subdirectory(
    glfw
)

add_subdirectory(
    ImGuiFileBrowser/FileBrowser
)

set(WITH_GEOTIFF FALSE)
set(WITH_LASZIP FALSE)
add_subdirectory(
    libLAS
)
    
