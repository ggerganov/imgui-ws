 [![Build Status](https://travis-ci.org/ggerganov/imgui-ws.svg?branch=master)](https://travis-ci.org/ggerganov/imgui-ws?branch=master)

# imgui-ws

[Dear ImGui](https://github.com/ocornut/imgui) over WebSockets

This is a small library that allows to stream a Dear ImGui scene to multiple WebSocket clients at once. This is achieved by sending Dear ImGui's DrawData structure which is then rendered in the browser using WebGL. To reduce the amount of network traffic, we send only the diffs between sequential frames (for more info - see [#1](../../issues/1/)).

## Live examples

These are sample applications using **imgui-ws**, running on a [Linode 2GB](https://www.linode.com/pricing) instance:

| Example | URL | Description |
| ------- | --- | ----------- |
| [demo-null](https://github.com/ggerganov/imgui-ws/tree/master/examples/demo-null) | http://85.90.246.132:5001/ | Dear ImGui's demo app |
| [basic-null](https://github.com/ggerganov/imgui-ws/tree/master/examples/basic-null) | http://85.90.246.132:5002/ | Basic read-only data visualization |

## Tools

There are a few tools that help with the development and the optimization of the ImDrawData compression:

- [record-sdl2](https://github.com/ggerganov/imgui-ws/tree/master/tools/record-sdl2)

  Record the ImDrawData for all rendered frames in a Dear ImGui session

      ./bin/record-sdl2 session.imdd

  ---

- [replay-sdl2](https://github.com/ggerganov/imgui-ws/tree/master/tools/replay-sdl2)

  Replay a session recorded with the **record-sdl2** tool

      ./bin/replay-sdl2 session.imdd

  ---

- [compressor-benchmark](https://github.com/ggerganov/imgui-ws/tree/master/tools/compressor-benchmark)

  Run the available ImDrawData compression algorithms on various pre-recorded Dear ImGui sessions. Reports compression ratio, average required bandwidth and cpu utilization:

      ./bin/compressor-benchmark session0.imdd [session1.imdd] [session2.imdd] [...]

  ---

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

## Dependencies

 - [uWebSockets](https://github.com/uNetworking/uWebSockets)

## Screenshot 

 <a href="https://i.imgur.com/TVxj8cf.png" target="_blank">![imgui-ws](https://i.imgur.com/TVxj8cf.png)</a>

## Credits
 - This project is heavily inspired by https://github.com/JordiRos/remoteimgui
 - Most of the JS code is taken from https://github.com/flyover/imgui-js
