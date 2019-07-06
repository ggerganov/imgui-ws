#include "imgui/imgui.h"
#include "imgui-ws/imgui-ws.h"

#include "common.h"

#include <map>

struct State {
    bool showDemoWindow = true;

    // client control management
    struct ClientData {
        bool hasControl = false;

        std::string ip = "---";
    };

    // client control
    float tControl_s = 10.0f;
    float tControlNext_s = 0.0f;

    int controlIteration = 0;
    int curIdControl = -1;
    std::map<int, ClientData> clients;

    // client input
    ImVec2 lastMousePos = { 0.0, 0.0 };
    bool  lastMouseDown[5];
    float lastMouseWheel;
    float lastMouseWheelH;

    void handle(ImGuiWS::Event && event);
    void update();
};

int main(int argc, char ** argv) {
    printf("Usage: %s [port] [http-root]\n", argv[0]);

    int port = 5000;
    std::string httpRoot = "../examples";

    if (argc > 1) port = atoi(argv[1]);
    if (argc > 2) httpRoot = argv[2];

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    // setup imgui-ws
    ImGuiWS imguiWS;
    imguiWS.init(port, (httpRoot + "/headless-with-input").c_str());

    // prepare font texture
    {
        unsigned char * pixels;
        int width, height;
        ImGui::GetIO().Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
        imguiWS.setTexture(1, width, height, (const char *) pixels);
    }

    VSync vsync;
    State state;

    while (true) {
        // websocket event handling
        auto events = imguiWS.takeEvents();
        for (auto & event : events) {
            state.handle(std::move(event));
        }
        state.update();

        io.DisplaySize = ImVec2(1200, 800);
        io.DeltaTime = vsync.delta_s();

        ImGui::NewFrame();

        // render stuff
        if (state.showDemoWindow) {
            ImGui::ShowDemoWindow(&state.showDemoWindow);
        }

        // debug window
        {
            ImGui::Begin("Hello, world!");
            ImGui::Checkbox("Demo Window", &state.showDemoWindow);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // show connected clients
        ImGui::SetNextWindowPos({ 10, 10 } , ImGuiCond_Always);
        ImGui::SetNextWindowSize({ 400, 300 } , ImGuiCond_Always);
        ImGui::Begin((std::string("WebSocket clients (") + std::to_string(state.clients.size()) + ")").c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
        ImGui::Text(" Id   Ip addr");
        for (auto & [ cid, client ] : state.clients) {
            ImGui::Text("%3d : %s", cid, client.ip.c_str());
            if (client.hasControl) {
                ImGui::SameLine();
                ImGui::TextDisabled(" [has control for %4.2f seconds]", state.tControlNext_s - ImGui::GetTime());
            }
        }
        ImGui::End();

        // generate ImDrawData
        ImGui::Render();

        // store ImDrawData for asynchronous dispatching to WS clients
        imguiWS.setDrawData(ImGui::GetDrawData());

        // if not clients are connected, just sleep to save CPU
        do {
            vsync.wait();
        } while (imguiWS.nConnected() == 0);
    }

    ImGui::DestroyContext();

    return 0;
}

void State::handle(ImGuiWS::Event && event) {
    switch (event.type) {
        case ImGuiWS::Event::Connected:
            {
                clients[event.clientId].ip = event.ip;
            }
            break;
        case ImGuiWS::Event::Disconnected:
            {
                clients.erase(event.clientId);
            }
            break;
        case ImGuiWS::Event::MouseMove:
            {
                if (event.clientId == curIdControl) {
                    lastMousePos.x = event.mouse_x;
                    lastMousePos.y = event.mouse_y;
                }
            }
            break;
        case ImGuiWS::Event::MouseDown:
            {
                if (event.clientId == curIdControl) {
                    lastMouseDown[event.mouse_but] = true;
                }
            }
            break;
        case ImGuiWS::Event::MouseUp:
            {
                if (event.clientId == curIdControl) {
                    lastMouseDown[event.mouse_but] = false;
                }
            }
            break;
        default:
            {
                printf("Unknown input event\n");
            }
    }
}

void State::update() {
    if (clients.size() > 0 && (clients.find(curIdControl) == clients.end() || ImGui::GetTime() > tControlNext_s)) {
        if (clients.find(curIdControl) != clients.end()) {
            clients[curIdControl].hasControl = false;
        }
        int k = ++controlIteration % clients.size();
        auto client = clients.begin();
        std::advance(client, k);
        client->second.hasControl = true;
        curIdControl = client->first;
        tControlNext_s = ImGui::GetTime() + tControl_s;
    }

    if (clients.size() == 0) {
        curIdControl = -1;
    }

    if (curIdControl > 0) {
        ImGui::GetIO().MousePos = lastMousePos;
        ImGui::GetIO().MouseDown[0] = lastMouseDown[0];
        ImGui::GetIO().MouseDown[1] = lastMouseDown[1];
        ImGui::GetIO().MouseDown[2] = lastMouseDown[2];
        ImGui::GetIO().MouseDown[3] = lastMouseDown[3];
        ImGui::GetIO().MouseDown[4] = lastMouseDown[4];
    }
}
