#include "Game.h"

Game::Game(std::string name) {
    this->name = name;
    loader = new Loader;
}

void Game::Init_SDL2_ImGUI() {
    render_context.Init(name);
}

void Game::Shutdown() {
    render_context.Destroy();
    delete loader;
}

void Loader::LoadUI(std::string ui_folderpath, std::list<Screen>& screens)
{

}
