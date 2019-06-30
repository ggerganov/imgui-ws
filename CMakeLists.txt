cmake_minimum_required (VERSION 3.10)
project(imgui-ws)

set(CMAKE_EXPORT_COMPILE_COMMANDS "on")

find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif ()

add_subdirectory(incppect)
add_subdirectory(src)
