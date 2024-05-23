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

if (SIMVIEW_WITH_ASSIMP)
    add_subdirectory(
        assimp
    )
endif()

add_subdirectory(
    freetype
)