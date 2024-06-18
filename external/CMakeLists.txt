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
    option(BUILD_SHARED_LIBS "" OFF)
    option(ASSIMP_BUILD_TESTS "" OFF)
    option(ASSIMP_BUILD_ZLIB "" ON)

    add_subdirectory(
        assimp
    )
endif()

add_subdirectory(
    freetype
)