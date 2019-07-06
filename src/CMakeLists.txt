file(READ "imgui-ws.js" src_imgui-ws_js)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/common.h.in ${CMAKE_CURRENT_SOURCE_DIR}/common.h @ONLY)

add_library(imgui-ws SHARED
    imgui-ws.cpp
    )

target_include_directories(imgui-ws PUBLIC
    ../include
    )

target_link_libraries(imgui-ws PRIVATE
    imgui
    incppect
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    )
