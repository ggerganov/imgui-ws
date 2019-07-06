add_executable(headless-with-input main.cpp)
target_include_directories(headless-with-input PRIVATE ..)
target_link_libraries(headless-with-input PRIVATE imgui imgui-ws)
