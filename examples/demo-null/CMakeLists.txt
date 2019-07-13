add_executable(demo-null main.cpp)
target_include_directories(demo-null PRIVATE ..)
target_link_libraries(demo-null PRIVATE imgui imgui-ws)
