if (APPLE)
    set(ADDITIONAL_LIBRARIES "-framework Cocoa")
else (APPLE)
    unset(ADDITIONAL_LIBRARIES)
endif (APPLE)

add_executable(imgui-ws
    main.cpp
    ../imgui/imgui.cpp
    ../imgui/imgui_draw.cpp
    ../imgui/imgui_demo.cpp
    ../imgui/imgui_widgets.cpp
    ../imgui/examples/libs/gl3w/GL/gl3w.c
    ../imgui/examples/imgui_impl_sdl.cpp
    ../imgui/examples/imgui_impl_opengl3.cpp
    )

target_include_directories(imgui-ws PRIVATE
    ../imgui
    ../imgui/examples
    ../imgui/examples/libs/gl3w
    ${SDL2_INCLUDE_DIRS}
    )

target_link_libraries(imgui-ws PRIVATE
    incppect
    ${SDL2_LIBRARIES}
    ${ADDITIONAL_LIBRARIES}
    )
