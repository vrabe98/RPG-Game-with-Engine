#include "Actions.h"

int main(int, char**)
{
    try {
        game.Init_SDL2_ImGUI();
        game.setActions(actions);
        game.setDB(db);
        game.Load("Data/folder_paths.json");
        game.Play();
    }
    catch (std::exception e) {
        printf(e.what());
        game.Shutdown();
        return -1;
    }

    game.Shutdown();

    return 0;
}