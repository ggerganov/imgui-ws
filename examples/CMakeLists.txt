if (IMGUI_WS_BUILD_HEADLESS_EXAMPLES)
    add_subdirectory(headless)
    add_subdirectory(headless-with-input)
endif()

if (IMGUI_WS_BUILD_SDL2_EXAMPLES)
    add_subdirectory(sdl2-basic)
endif()
