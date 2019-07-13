add_executable(record-sdl2 main.cpp)
target_include_directories(record-sdl2 PRIVATE ..)
target_link_libraries(record-sdl2 PRIVATE imgui imgui-sdl2 imgui-ws)
