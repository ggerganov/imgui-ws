add_executable(sdl2-basic main.cpp)
target_include_directories(sdl2-basic PRIVATE ..)
target_link_libraries(sdl2-basic PRIVATE imgui imgui-sdl2 imgui-ws)
