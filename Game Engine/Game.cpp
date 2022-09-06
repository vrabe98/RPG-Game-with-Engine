#include "Game.h"

/*
    Game class method definitions
*/
Game::Game(std::string name) {
    this->name = name;
    loader = new Loader;
}

RenderContext* Game::getRenderContext(){
    return &render_context;
}

void Game::Init_SDL2_ImGUI() {
    render_context.Init(name);
}

void Game::Load(std::string data_paths_json)
{
    json paths = json::parse(std::ifstream(data_paths_json));
    loader->LoadUI(paths["UI"].get<std::string>(),screens,&render_context);
}

void Game::Shutdown() {
    render_context.Destroy();
    for (Screen* scr : screens) {
        delete scr;
    }
    delete loader;
}

/*
    Loader class method definitions
*/
void Loader::LoadUI(std::string ui_folderpath, std::vector<Screen*>& screens,RenderContext* context){
    for (auto& entry : std::filesystem::directory_iterator(ui_folderpath)) {
        json data = json::parse(std::ifstream(entry.path()));
        Screen* screen = new Screen(context);
        int ind = 0;
        for (auto& element : data) {
            //a window element is followed by a list of renderables, there can be multiple windows in 1 file
            if (element["type"].get<std::string>() == "window") {
                Window* wnd = new Window(data, ind, context);
                screen->AddWindow(wnd);
            }
            ind++;
        }
        screens.push_back(screen);
    }
}
