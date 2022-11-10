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

void Game::Load(std::string game_metadata_json)
{
    json metadata = json::parse(std::ifstream(game_metadata_json));
    json paths = metadata["Data file paths"];
    loader->LoadMaps(paths["Maps"], metadata["Start map ID"].get<int>(), render_context);
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
        for (auto& element : data) {
            if (element["type"].get<std::string>() == "window") {
                std::shared_ptr<Window> wnd = std::make_shared<Window>(element, context,actions);
                screen->AddWindow(wnd);
            }
            else if (element["type"].get<std::string>() == "map_view") {
                std::shared_ptr<Map_view> view = std::make_shared<Map_view>(element, db->get_current_map_ptr(),db->get_maps_ptr(),context);
                screen->AddRenderable(view);
            }
            else if(misc_rnd.contains(element["type"].get<std::string>())){
                std::shared_ptr<Texture> tex = std::make_shared<Texture>(element, context);
                screen->AddRenderable(tex);
            }
        }
        db->screens.push_back(screen);
    }
}

void Loader::LoadMaps(json map_paths, int start_map, std::unique_ptr<RenderContext>& context){
    std::string metadata_paths = map_paths["Metadata"].get<std::string>();
    std::string tilemap_paths = map_paths["Tilemaps"].get<std::string>();
    std::string tileset_paths = map_paths["Tilesets"].get<std::string>();

    db->current_map = start_map;

    for (auto& entry : std::filesystem::directory_iterator(map_paths["Metadata"].get<std::string>())) {
        json data = json::parse(std::ifstream(entry.path()));
        std::shared_ptr<Map> map = std::make_shared<Map>(data, context);
        map->set_pos_ptr(db->get_mainchar_pos());
        db->add_map(map);
    }
}

/*
    Database class method definitions
*/

std::shared_ptr<Screen> Database::getScreen(int scr){
    return screens[scr];
}

int Database::get_current_map_nr() {
    return current_map;
}

std::shared_ptr<Map>& Database::get_current_map_ptr() {
    update_current_map();
    return current_map_ptr;
}

void Database::update_current_map() {
    current_map_ptr= (*maps)[current_map];
}

void Database::update_current_map(int nr) {
    if (nr >= 0 && nr < maps->size()) {
        current_map = nr;
        current_map_ptr = (*maps)[current_map];
    }
}

//PLACEHOLDER
std::shared_ptr<Coordinate>& Database::get_mainchar_pos() {
    return mainchar_pos;
}

void Database::add_map(std::shared_ptr<Map> map) {
    maps->push_back(map);
}

std::unique_ptr<std::vector<std::shared_ptr<Map>>>& Database::get_maps_ptr() {
    return maps;
}

Database::Database() {
    maps = std::make_unique<std::vector<std::shared_ptr<Map>>>();
    mainchar_pos = std::make_shared<Coordinate>(0, 0);
    current_map = MAP_NONE;
}