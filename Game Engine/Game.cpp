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
    loader->LoadMaps(paths["Maps"], render_context);
    loader->LoadCharacters(paths["Characters"].get<std::string>(), render_context);
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
                std::shared_ptr<Map_view> view = std::make_shared<Map_view>(element, db->get_current_map_ptr(),
                                                    db->get_maps_ptr(),db->get_npcs(),db->get_main_char(),context);
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

void Loader::LoadMaps(json map_paths,std::unique_ptr<RenderContext>& context){
    for (auto& entry : std::filesystem::directory_iterator(map_paths["Metadata"].get<std::string>())) {
        json data = json::parse(std::ifstream(entry.path()));
        std::shared_ptr<Map> map = std::make_shared<Map>(data, context);
        db->add_map(map,map->get_id());
    }
}

void Loader::LoadCharacters(std::string chars_paths, std::unique_ptr<RenderContext>& context) {
    for (auto& entry : std::filesystem::directory_iterator(chars_paths)) {
        json data = json::parse(std::ifstream(entry.path()));
        for (auto& character : data) {
            if (character["type"].get<std::string>() == "main") {
                int map_id = character["map_id"];
                std::shared_ptr<Map> map = (*db->get_maps_ptr())[map_id];
                std::shared_ptr<Main_character> main_char = std::make_shared<Main_character>(character, map->get_tile_dim(),context);
                db->set_main_char(main_char);
            }
            else if (character["type"].get<std::string>() == "NPC") {
                int map_id = character["map_id"];
                std::shared_ptr<Map> map = (*db->get_maps_ptr())[map_id];
                std::shared_ptr<Character> npc = std::make_shared<Character>(character, map->get_tile_dim(),context);
                db->add_npc(npc);
            }
        }
    }
}

/*
    Database class method definitions
*/

std::shared_ptr<Screen> Database::getScreen(int scr){
    return screens[scr];
}

int Database::get_current_map_nr() {
    return main_char->get_current_map_id();
}

std::shared_ptr<Map>& Database::get_current_map_ptr() {
    update_current_map();
    return current_map_ptr;
}

void Database::update_current_map() {
    current_map_ptr = (*maps)[main_char->get_current_map_id()];
}

std::shared_ptr<Main_character> Database::get_main_char(){
    return main_char;
}

std::unique_ptr<std::vector<std::shared_ptr<Character>>>& Database::get_npcs(){
    return npcs;
}

void Database::force_update_current_map(uint16_t nr) {
    if (nr >= 0 && nr < maps->size()) {
        main_char->set_current_map_id(nr);
        current_map_ptr = (*maps)[nr];
    }
}

//PLACEHOLDER
std::shared_ptr<Coordinate>& Database::get_mainchar_pos() {
    return mainchar_pos;
}

void Database::add_map(std::shared_ptr<Map> map,uint16_t pos) {
    if (maps->size() < pos+1) {
        maps->resize(pos+1);
        (*maps)[pos] = map;
    }
}

void Database::add_npc(std::shared_ptr<Character> npc) {        //NEED TO CHANGE THIS TO SORT BY MAP ID
    npcs->push_back(npc);
}

void Database::set_main_char(std::shared_ptr<Main_character> main_char) {
    this->main_char = main_char;
    this->mainchar_pos = main_char->get_pos();
    for (auto& map : *maps) {
        map->set_pos_ptr(mainchar_pos);
    }
}

std::unique_ptr<std::vector<std::shared_ptr<Map>>>& Database::get_maps_ptr() {
    return maps;
}

Database::Database() {
    maps = std::make_unique<std::vector<std::shared_ptr<Map>>>();
    npcs = std::make_unique<std::vector<std::shared_ptr<Character>>>();
    mainchar_pos = std::make_shared<Coordinate>(0, 0);
}