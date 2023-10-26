 [![Actions Status](https://github.com/ggerganov/imgui-ws/workflows/CI/badge.svg)](https://github.com/ggerganov/imgui-ws/actions)

# imgui-ws

[Dear ImGui](https://github.com/ocornut/imgui) over WebSockets

This is a small library that allows to stream a Dear ImGui scene to multiple WebSocket clients at once. This is achieved by sending Dear ImGui's DrawData structure which is then rendered in the browser using WebGL. To reduce the amount of network traffic, we send only the diffs between sequential frames (for more info - see [#1](../../issues/1/)).

## Live examples

These are sample applications using **imgui-ws**, running on a [Linode 2GB](https://www.linode.com/pricing) instance:

| Example | URL | Description |
| ------- | --- | ----------- |
| [demo-null](https://github.com/ggerganov/imgui-ws/tree/master/examples/demo-null) | http://imgui-ws.ggerganov.com:5001/ | Dear ImGui's demo app |
| [basic-null](https://github.com/ggerganov/imgui-ws/tree/master/examples/basic-null) | http://imgui-ws.ggerganov.com:5002/ | Basic read-only data visualization |
| [textures-null](https://github.com/ggerganov/imgui-ws/tree/master/examples/textures-null) | http://imgui-ws.ggerganov.com:5003/ | Textures example |

Note that these examples are "headless". This means that no graphics backend (like OpenGL, SDL, Vulkan, etc) is needed on the server, because it does not render anything. The application just generates the Dear ImGui draw data and sends it to any connected WebSocket clients where the actual rendering occurs. This has the advantage of being able to run applications with Dear ImGui interface on almost any kind of hardware, even one without a monitor or a graphics card. For example, the applications can run on a [Raspberry Pi](https://www.raspberrypi.org), or on a cloud VM as in the examples above.

## Tools

There are a few tools that help with the development and the optimization of the ImDrawData compression.
To build the SDL-based tools, use `cmake -DIMGUI_WS_SUPPORT_SDL2=ON`.

- [record-sdl2](https://github.com/ggerganov/imgui-ws/tree/master/tools/record-sdl2)

  Record the ImDrawData for all rendered frames in a Dear ImGui session

      ./bin/record-sdl2 session.imdd

- [replay-sdl2](https://github.com/ggerganov/imgui-ws/tree/master/tools/replay-sdl2)

  Replay a session recorded with the **record-sdl2** tool

      ./bin/replay-sdl2 session.imdd

- [compressor-benchmark](https://github.com/ggerganov/imgui-ws/tree/master/tools/compressor-benchmark)

  Run the available ImDrawData compression algorithms on various pre-recorded Dear ImGui sessions. Reports compression ratio, average required bandwidth and cpu utilization:

      ./bin/compressor-benchmark session0.imdd [session1.imdd] [session2.imdd] [...]

## Building & running

```bash
# build
git clone https://github.com/ggerganov/imgui-ws --recursive
cd imgui-ws && mkdir build && cd build
cmake ..
make

# running
./bin/demo-null 5000
```

Open some browsers and point them to http://localhost:5000/

## Using in your application

A simple way of using this library in your cmake-based application is
to add the following to your `CMakeLists.txt`:

```cmake
include(FetchContent)
FetchContent_Declare(imgui-ws
    GIT_REPOSITORY    https://github.com/ggerganov/imgui-ws.git
    GIT_TAG           eab5d6b169ed3e5f8d26c4a6cd9a3271dd6e2dda
    GIT_SUBMODULES_RECURSE YES
    )
FetchContent_MakeAvailable(imgui-ws)
set(INCPPECT_DEBUG NO)
# add_executable(«your_executable» «sources»)
# ...
target_link_libraries(«your_executable» imgui imgui-ws)
install(TARGETS incppect uWS)
```

## Dependencies

 - [uWebSockets](https://github.com/uNetworking/uWebSockets)
 - [SDL2](https://www.libsdl.org) - *optional, used by some of the examples and tools*

## Screenshot 

 <a href="https://i.imgur.com/TVxj8cf.png" target="_blank">![imgui-ws](https://i.imgur.com/TVxj8cf.png)</a>

## Credits
 - This project is heavily inspired by https://github.com/JordiRos/remoteimgui
 - Most of the JS code is taken from https://github.com/flyover/imgui-js
