add_executable(textures-null main.cpp)
target_include_directories(textures-null PRIVATE ..)
target_link_libraries(textures-null PRIVATE imgui imgui-ws)
