add_executable(replay-sdl2 main.cpp)
target_include_directories(replay-sdl2 PRIVATE ..)
target_link_libraries(replay-sdl2 PRIVATE imgui imgui-sdl2 imgui-ws)
