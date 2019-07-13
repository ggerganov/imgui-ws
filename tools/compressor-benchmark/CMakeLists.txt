add_executable(compressor-benchmark main.cpp)
target_include_directories(compressor-benchmark PRIVATE ..)
target_link_libraries(compressor-benchmark PRIVATE imgui imgui-ws)
