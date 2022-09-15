#include "Game.h"

/*
    Game class method definitions
*/

Game::Game(std::string name) {
    this->name = name;
    loader = std::make_unique<Loader>();
}

std::shared_ptr<RenderContext> Game::getRenderContext(){
    return render_context;
}

void Game::Init_SDL2_ImGUI() {
    render_context = std::make_shared<RenderContext>(name);
}

void Game::Load(std::string data_paths_json)
{
    json paths = json::parse(std::ifstream(data_paths_json));
    loader->LoadUI(paths["UI"].get<std::string>(),screens,render_context);
    loader.reset();     //delete loader after the loading process is done
}

void Game::Shutdown() {
    for (std::shared_ptr<Screen> scr : screens) {
        scr.reset();
    }
    render_context.reset();
}

/*
    Loader class method definitions
*/

void Loader::LoadUI(std::string ui_folderpath, std::vector<std::shared_ptr<Screen>>& screens,std::shared_ptr<RenderContext> context){
    for (auto& entry : std::filesystem::directory_iterator(ui_folderpath)) {
        json data = json::parse(std::ifstream(entry.path()));
        std::shared_ptr<Screen> screen = std::make_shared<Screen>(context);
        for (auto& element : data) {    //supports dear imGUI windows with widgets or other renderables
            if (element["type"].get<std::string>() == "window") {
                std::shared_ptr<Window> wnd = std::make_shared<Window>(element, context);
                screen->AddWindow(wnd);
            }
        }
        screens.push_back(screen);
    }
}
