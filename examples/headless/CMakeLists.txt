add_executable(headless main.cpp)
target_include_directories(headless PRIVATE ..)
target_link_libraries(headless PRIVATE imgui imgui-ws)
