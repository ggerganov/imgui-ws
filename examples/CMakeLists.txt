if (IMGUI_WS_BUILD_EXAMPLES_NULL)
    add_subdirectory(basic-null)
    add_subdirectory(demo-null)
endif()

if (IMGUI_WS_SUPPORT_SDL2 AND IMGUI_WS_BUILD_EXAMPLES_SDL2)
    add_subdirectory(basic-sdl2)
endif()
