#include "Game.h"

Game::Game(std::string name) {
    this->name = name;
    loader = new Loader;
}

void Game::Init_SDL2_ImGUI() {
    render_context.Init(name);
}

void Game::Load(std::string data_paths_json)
{
    json paths = json::parse(std::ifstream(data_paths_json));
    for (auto& entry : std::filesystem::directory_iterator(paths["UI"].get<std::string>())) {
        json data=json::parse(std::ifstream(entry.path()));
        Screen* screen=new Screen(&render_context);
        int ind = 0;
        for (auto& element : data) {
            //a window element is followed by a list of renderables, there can be multiple windows in 1 file
            if (element["type"].get<std::string>() == "window") {
                Window* wnd = new Window(data, ind, &render_context);
                screen->AddWindow(wnd);
            }
            ind++;
        }
        screens.push_back(screen);
    }
}

void Game::Shutdown() {
    render_context.Destroy();
    for (Screen* scr : screens) {
        delete scr;
    }
    delete loader;
}

void Loader::LoadUI(std::string ui_folderpath, std::list<Screen>& screens){

}
