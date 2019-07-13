if (IMGUI_WS_BUILD_TOOLS)
    add_subdirectory(compressor-benchmark)
endif ()

if (IMGUI_WS_SUPPORT_SDL2 AND IMGUI_WS_BUILD_TOOLS)
    add_subdirectory(record-sdl2)
    add_subdirectory(replay-sdl2)
endif()
