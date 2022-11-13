#include "CustomGame.h"

CustomGame game("Joc");
std::shared_ptr<Database> db = std::make_shared<Database>();

void CustomGame::Play(){
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        Coordinate dir = { 0,0 };
        const uint8_t* keys = SDL_GetKeyboardState(NULL);
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type==SDL_QUIT||(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(render_context->window)))
                done = true;
        }
        dir.y = -keys[SDL_SCANCODE_UP] + keys[SDL_SCANCODE_DOWN];
        dir.x = -keys[SDL_SCANCODE_LEFT] + keys[SDL_SCANCODE_RIGHT];
        moveMain(dir);
        if(!done) done=!db->getScreen(game_state)->Render();
    }
}

void CustomGame::moveMain(Coordinate vel) {
    Coordinate oldpos = *(db->get_mainchar_pos());
    std::shared_ptr<Coordinate>& pos = db->get_mainchar_pos();
    db->get_main_char()->move(vel,char_accel,char_decel,max_vel,db->get_current_map_ptr()->get_px_size());
    if (!db->get_current_map_ptr()->can_access()) {
        db->get_main_char()->stop_move();
        db->get_main_char()->force_move(oldpos);
    }
}