if (IMGUI_WS_ALL_WARNINGS_3RD_PARTY)
    if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    else()
        # todo : windows
    endif()
endif()

if (APPLE)
    set(ADDITIONAL_LIBRARIES "-framework Cocoa")
else (APPLE)
    unset(ADDITIONAL_LIBRARIES)
endif (APPLE)

add_library(imgui SHARED
    imgui/imgui/imgui.cpp
    imgui/imgui/imgui_draw.cpp
    imgui/imgui/imgui_demo.cpp
    imgui/imgui/imgui_widgets.cpp
    imgui/imgui/imgui_tables.cpp
    )

target_include_directories(imgui INTERFACE
    imgui
    )

target_include_directories(imgui PRIVATE
    imgui/imgui
    )

target_link_libraries(imgui PRIVATE
    ${ADDITIONAL_LIBRARIES}
    )

if (IMGUI_WS_SUPPORT_SDL2)
    find_package(OpenGL REQUIRED)
    find_package(SDL2 REQUIRED)
    string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

    add_library(imgui-sdl2 SHARED
        imgui/imgui/examples/libs/gl3w/GL/gl3w.c
        imgui/imgui/examples/imgui_impl_sdl.cpp
        imgui/imgui/examples/imgui_impl_opengl3.cpp
        )

    target_include_directories(imgui-sdl2 PUBLIC
        imgui/imgui/examples/libs/gl3w
        ${SDL2_INCLUDE_DIRS}
        )

    target_include_directories(imgui-sdl2 PRIVATE
        imgui/imgui
        imgui/imgui/examples
        )

    target_link_libraries(imgui-sdl2 PUBLIC
        ${SDL2_LIBRARIES}
        )

    target_link_libraries(imgui-sdl2 PRIVATE
        imgui
        ${OPENGL_LIBRARIES}
        ${CMAKE_DL_LIBS}
        ${CMAKE_THREAD_LIBS_INIT}
        ${ADDITIONAL_LIBRARIES}
        )
endif()

add_subdirectory(incppect)
