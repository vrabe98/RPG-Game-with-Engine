#include "CustomGame.h"

CustomGame game("Joc");
std::shared_ptr<Database> db = std::make_shared<Database>();

void CustomGame::Play(){
    ImVec4 bkg_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        Button button_play, button_exit;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(render_context->window))
                done = true;
        }
        done=!db->getScreen(game_state)->Render();
    }
}