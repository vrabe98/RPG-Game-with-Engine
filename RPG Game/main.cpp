#include <iostream>
#include <fstream>
#include <json.hpp>
#include "CustomGame.h"

using json = nlohmann::json;

int main(int, char**)
{
    CustomGame game("Joc");

    try {
        game.Init_SDL2_ImGUI();
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