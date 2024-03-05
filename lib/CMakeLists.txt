add_subdirectory(
    glad
)

add_subdirectory(
    glfw
)

add_subdirectory(
    nativefiledialog-extended
)

add_subdirectory(
    spdlog
)

set(ASSIMP_BUILD_TESTS off)
add_subdirectory(
    assimp
)