#include "imgui-extra/imgui_impl.h"

#include "common.h"

#include <SDL.h>
#include <SDL_opengl.h>

// Main code
int main(int argc , char ** argv) {
    printf("Usage: %s session.imdd\n", argv[0]);

    if (argc < 2) {
        return -1;
    }

    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    ImGui_PreInit();

    int windowX = 1080;
    int windowY = 720;

    const char * windowTitle = "imgui-ws : record-sdl2";
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window * window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowX, windowY, window_flags);

    void * gl_context = SDL_GL_CreateContext(window);

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    ImGui_Init(window, gl_context);

    ImGui::StyleColorsDark();

    // record the entire Dear ImGui session in this object
    Session session;

    // Main loop
    bool done = false;
    while (!done) {
        // process SDL window events
        {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                ImGui_ProcessEvent(&event);
                if (event.type == SDL_QUIT)
                    done = true;
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                    done = true;
            }
        }

        // Start the Dear ImGui frame
        ImGui_NewFrame(window);
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({ 20, 20 });
        ImGui::SetNextWindowSize({ 400, 100 });
        ImGui::Begin("Hello, world!");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::SetNextWindowPos({ 20, 360 });
        ImGui::ShowAboutWindow();

        ImGui::ShowDemoWindow();
        ImGui::EndFrame();

        // generate ImDrawData
        ImGui::Render();

        // render SDL2 window
        {
            glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(window);
        }

        session.addFrame(ImGui::GetDrawData());
    }

    printf("[+] Saving recorded session to '%s' ...\n", argv[1]);
    session.printInfo();
    session.save(argv[1]);

    // Cleanup
    ImGui_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
