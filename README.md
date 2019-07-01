# imgui-ws

[Dear ImGui](https://github.com/ocornut/imgui) over WebSockets

This is a small library that allows to stream a Dear ImGui scene to multiple WebSocket clients at once. This is achieved by sending Dear ImGui's DrawData structure which is then rendered in the browser using WebGL. To reduce the amount of network traffic, we send only the diffs between sequential frames.

## Usage

```
#include "imgui-ws.h"
...
// initialize an instance of the http/WS server
ImGuiWS imguiWS;
imguiWS.init(port, pathHttp);
...

// set the ImGui font texture, so it can be transmitted to connecting clients
ImGui::GetIO().Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
imguiWS.setTexture(1, width, height, (const char *) pixels);

...
// set the ImGui DrawData, so it can be streamed to the WS clients
ImGui::Render();
imguiWS.setDrawData(ImGui::GetDrawData());

```

## Building & running

```
# build
git clone https://github.com/ggerganov/imgui-ws --recursive
cd imgui-ws && mkdir build && cd build
cmake ..
make

# running
./src/imgui-ws
```

Open some browsers and point them to http://localhost:5000/

## Dependencies

 - [uWebSockets](https://github.com/uNetworking/uWebSockets)

## Screenshot 

 <a href="https://i.imgur.com/TVxj8cf.png" target="_blank">![imgui-ws](https://i.imgur.com/TVxj8cf.png)</a>

## Live demo

If you are lucky the following server might be up and running.

**Note:** the page can stream relatively big amount of traffic:

http://95.111.37.39:5000/

## Credits
 - This project is heavily inspired by https://github.com/JordiRos/remoteimgui
 - Most of the JS code is taken from https://github.com/flyover/imgui-js
