#include "CustomGame.h"

CustomGame game("Joc");
std::shared_ptr<Database> db = std::make_shared<Database>();

void CustomGame::Play(){
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(render_context->window))
                done = true;
        }
        if(!done) done=!db->getScreen(game_state)->Render();
    }
}