#include "CustomGame.h"

int main(int, char**)
{
    CustomGame game("Joc");

    try {
        game.Init_SDL2_ImGUI();
        game.Load("Data/folder_paths.json");
    }
    catch (std::runtime_error e) {
        printf(e.what());
        game.Shutdown();
        return -1;
    }

    game.Play();

    game.Shutdown();

    return 0;
}