#include <set>
#include "Game.h"

std::set<std::string> misc_rnd={"background","texture"};

/*
    Game class method definitions
*/

Game::Game(std::string name) {
    this->name = name;
    game_state = 0;
    loader = std::make_unique<Loader>();
}

std::unique_ptr<RenderContext>& Game::getRenderContext(){
    return render_context;
}

void Game::Init_SDL2_ImGUI() {
    render_context = std::make_unique<RenderContext>(name);
}

void Game::setActions(std::shared_ptr<actions_map>& actions) {
    loader->setActions(actions);
}

void Game::setDB(std::shared_ptr<Database>& db){
    this->db = db;
    loader->setDB(db);
}

void Game::setState(int state) {
    this->game_state = state;
}

std::shared_ptr<Database> Game::getDB(){
    return db;
}

void Game::Load(std::string data_paths_json)
{
    json paths = json::parse(std::ifstream(data_paths_json));
    loader->LoadUI(paths["UI"].get<std::string>(),render_context);
    loader.reset();     //delete loader after the loading process is done
}

void Game::Shutdown() {
    for (auto& scr : db->screens) {
        scr.reset();
    }
    render_context.reset();
}

/*
    Loader class method definitions
*/

void Loader::setActions(std::shared_ptr<actions_map>& actions) {
    this->actions = actions;
}

void Loader::setDB(std::shared_ptr<Database>& db){
    this->db = db;
}

void Loader::LoadUI(std::string ui_folderpath,std::unique_ptr<RenderContext>& context){
    for (auto& entry : std::filesystem::directory_iterator(ui_folderpath)) {
        json data = json::parse(std::ifstream(entry.path()));
        std::shared_ptr<Screen> screen = std::make_shared<Screen>(context);
        for (auto& element : data) {    //supports dear imGUI windows with widgets or other renderables
            if (element["type"].get<std::string>() == "window") {
                std::shared_ptr<Window> wnd = std::make_shared<Window>(element, context,actions);
                screen->AddWindow(wnd);
            }
            else if(misc_rnd.contains(element["type"].get<std::string>())){
                std::shared_ptr<Texture> tex = std::make_shared<Texture>(element, context);
                screen->AddRenderable(tex);
            }
        }
        db->screens.push_back(screen);
    }
}

/*
    Database class method definitions
*/

std::shared_ptr<Screen> Database::getScreen(int scr){
    return screens[scr];
}
