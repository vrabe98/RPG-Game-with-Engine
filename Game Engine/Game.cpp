#include "Game.h"

Game::Game(std::string name) {
    this->name = name;
}

void Game::Init_SDL2_ImGUI() {
    render_context.Init(name);
}

void Game::Shutdown() {
    render_context.Destroy();
}