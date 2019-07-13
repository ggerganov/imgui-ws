add_executable(basic-null main.cpp)
target_include_directories(basic-null PRIVATE ..)
target_link_libraries(basic-null PRIVATE imgui imgui-ws)
