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

    // load a recorder Dear ImGui session
    Session session;
    if (session.load(argv[1]) == false) {
        fprintf(stderr, "[E] Failed reading the file. Probably not an ImDrawData session file?\n");
        return -1;
    }

    ImDrawData drawData;
    std::vector<ImDrawList> drawLists;

    // Main loop
    int fid = 0;
    bool done = false;

    while (!done) {
        // process SDL window events
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    done = true;
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                    done = true;
            }
        }

        // make a dummy frame
        ImGui_NewFrame(window);
        ImGui::NewFrame();
        ImGui::EndFrame();
        ImGui::Render();

        // load the actual draw data for the frame from the recorded session
        if (session.getFrame(fid++, &drawData, drawLists, ImGui::GetDrawListSharedData()) == false) {
            break;
        }

        // render SDL2 window
        {
            glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_RenderDrawData(&drawData);
            SDL_GL_SwapWindow(window);
        }
    }

    // Cleanup
    ImGui_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
