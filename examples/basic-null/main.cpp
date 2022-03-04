#include "imgui/imgui.h"
#include "imgui-ws/imgui-ws.h"

#include "common.h"

int main(int argc, char ** argv) {
    printf("Usage: %s [port] [http-root]\n", argv[0]);

    int port = 5000;
    std::string httpRoot = "../examples";

    if (argc > 1) port = atoi(argv[1]);
    if (argc > 2) httpRoot = argv[2];

    // init dear imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    // setup imgui-ws
    ImGuiWS imguiWS;
    imguiWS.init(port, httpRoot + "/basic-null", { "", "index.html" });

    // prepare font texture
    {
        unsigned char * pixels;
        int width, height;
        ImGui::GetIO().Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
        imguiWS.setTexture(0, ImGuiWS::Texture::Type::Alpha8, width, height, (const char *) pixels);
    }

    VSync vsync;

    while (true) {
        io.DisplaySize = ImVec2(640, 480);
        io.DeltaTime = vsync.delta_s();

        ImGui::NewFrame();

        ImGui::SetNextWindowPos({ 20, 20 });
        ImGui::SetNextWindowSize({ 400, 100 });
        ImGui::Begin("Hello, world!");
        ImGui::Text("Connected clients: %d", imguiWS.nConnected());
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::SetNextWindowPos({ 20, 140 });
        ImGui::SetNextWindowSize({ 400, 200 });
        ImGui::Begin("Some measured data");
        {
            static int idx = 0;
            static int idx_last = 0;
            static float data[128];
            data[idx] = (0.8f*data[idx_last] + 0.2f*(rand()%100 + 50));
            idx_last = idx;
            if (++idx >= 128) idx = 0;
            ImGui::PlotHistogram("##signal", data, 128, idx, "Some signal", 0, FLT_MAX, ImGui::GetContentRegionAvail());
        }
        ImGui::End();

        ImGui::SetNextWindowPos({ 20, 360 });
        ImGui::ShowAboutWindow();

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
