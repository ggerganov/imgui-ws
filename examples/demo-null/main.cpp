#include "imgui/imgui.h"
#include "imgui-ws/imgui-ws.h"

#include "common.h"

#include <map>

struct State {
    State() {
        for (int i = 0; i < 512; ++i) {
            lastKeysDown[i] = false;
        }
    }

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
    bool  lastMouseDown[5] = { false, false, false, false, false };
    float lastMouseWheel = 0.0;
    float lastMouseWheelH = 0.0;

    std::string lastAddText = "";
    bool lastKeysDown[512];

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

    io.KeyMap[ImGuiKey_Tab]         = 43;
    io.KeyMap[ImGuiKey_LeftArrow]   = 80;
    io.KeyMap[ImGuiKey_RightArrow]  = 79;
    io.KeyMap[ImGuiKey_UpArrow]     = 82;
    io.KeyMap[ImGuiKey_DownArrow]   = 81;
    io.KeyMap[ImGuiKey_PageUp]      = 75;
    io.KeyMap[ImGuiKey_PageDown]    = 78;
    io.KeyMap[ImGuiKey_Home]        = 74;
    io.KeyMap[ImGuiKey_End]         = 77;
    io.KeyMap[ImGuiKey_Insert]      = 73;
    io.KeyMap[ImGuiKey_Delete]      = 76;
    io.KeyMap[ImGuiKey_Backspace]   = 8;
    io.KeyMap[ImGuiKey_Space]       = 32;
    io.KeyMap[ImGuiKey_Enter]       = 13;
    io.KeyMap[ImGuiKey_Escape]      = 27;
    io.KeyMap[ImGuiKey_A]           = 97;
    io.KeyMap[ImGuiKey_C]           = 99;
    io.KeyMap[ImGuiKey_V]           = 118;
    io.KeyMap[ImGuiKey_X]           = 120;
    io.KeyMap[ImGuiKey_Y]           = 121;
    io.KeyMap[ImGuiKey_Z]           = 122;

    io.MouseDrawCursor = true;

    ImGui::StyleColorsDark();
    ImGui::GetStyle().AntiAliasedFill = false;
    ImGui::GetStyle().AntiAliasedLines = false;
    ImGui::GetStyle().WindowRounding = 0.0f;
    ImGui::GetStyle().ScrollbarRounding = 0.0f;

    // setup imgui-ws
    ImGuiWS imguiWS;
    imguiWS.init(port, (httpRoot + "/demo-null").c_str());

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
                    lastMousePos.x = event.mouse_x;
                    lastMousePos.y = event.mouse_y;
                }
            }
            break;
        case ImGuiWS::Event::MouseUp:
            {
                if (event.clientId == curIdControl) {
                    lastMouseDown[event.mouse_but] = false;
                    lastMousePos.x = event.mouse_x;
                    lastMousePos.y = event.mouse_y;
                }
            }
            break;
        case ImGuiWS::Event::MouseWheel:
            {
                if (event.clientId == curIdControl) {
                    lastMouseWheelH = event.wheel_x;
                    lastMouseWheel  = event.wheel_y;
                }
            }
            break;
        case ImGuiWS::Event::KeyUp:
            {
                if (event.clientId == curIdControl) {
                    if (event.key > 0) {
                        lastKeysDown[event.key] = false;
                    }
                }
            }
            break;
        case ImGuiWS::Event::KeyDown:
            {
                if (event.clientId == curIdControl) {
                    if (event.key > 0) {
                        lastKeysDown[event.key] = true;
                    }
                }
            }
            break;
        case ImGuiWS::Event::KeyPress:
            {
                if (event.clientId == curIdControl) {
                    lastAddText.resize(1);
                    lastAddText[0] = event.key;
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
        ImGui::GetIO().MouseWheelH = lastMouseWheelH;
        ImGui::GetIO().MouseWheel = lastMouseWheel;
        ImGui::GetIO().MouseDown[0] = lastMouseDown[0];
        ImGui::GetIO().MouseDown[1] = lastMouseDown[1];
        ImGui::GetIO().MouseDown[2] = lastMouseDown[2];
        ImGui::GetIO().MouseDown[3] = lastMouseDown[3];
        ImGui::GetIO().MouseDown[4] = lastMouseDown[4];

        if (lastAddText.size() > 0) {
            ImGui::GetIO().AddInputCharactersUTF8(lastAddText.c_str());
        }

        for (int i = 0; i < 512; ++i) {
            ImGui::GetIO().KeysDown[i] = lastKeysDown[i];
        }

        lastMouseWheelH = 0.0;
        lastMouseWheel = 0.0;
        lastAddText = "";
    }
}
