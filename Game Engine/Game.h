#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include "Screen.h"

#define MAP_NONE -1

class Loader;
class Database;

/*
	Abstract class with a pure virtual method. This method's implementation has to be provided by the user,
	and it's role will be to host the main game loop.
*/

class Game
{
protected:
	int game_state;					//defines the game screen that is shown, corresponds to the json screen file number
	std::unique_ptr<Loader> loader;							//can theoretically be destroyed after loading
	std::shared_ptr<Database> db;
	std::unique_ptr<RenderContext> render_context;
	std::string name;
public:
	std::unique_ptr<RenderContext>& getRenderContext();
	void Init_SDL2_ImGUI();
	void Load(std::string);
	void Shutdown();
	void setActions(std::shared_ptr<actions_map>&);
	void setDB(std::shared_ptr<Database>&);
	void setState(int);
	std::shared_ptr<Database> getDB();
	virtual void Play() = 0;
	Game(std::string);
};

/*
	The Database class stores the information of the various game components.
	It also acts as an interface to modify the game's state.
*/

class Database {
	int current_map;
	std::shared_ptr<Coordinate> mainchar_pos;
	std::unique_ptr<std::vector<std::shared_ptr<Map>>> maps;
	std::shared_ptr<Map> current_map_ptr;
	std::vector<std::shared_ptr<Screen>> screens;
public:
	std::shared_ptr<Screen> getScreen(int);
	std::shared_ptr<Map>& get_current_map_ptr();
	int get_current_map_nr();
	void update_current_map();
	void update_current_map(int);
	std::shared_ptr<Coordinate>& get_mainchar_pos();	//PLACEHOLDER
	std::unique_ptr<std::vector<std::shared_ptr<Map>>>& get_maps_ptr();
	void add_map(std::shared_ptr<Map>,uint16_t);
	Database();
	friend class Loader;	//the Loader can access the private members
	friend class Game;		//the custom game class won't inherit this
};

/*
	The Loader class performs the loading of various game components, like:
		-UI elements
		-NPC data
		-Map data
		-Item data
		-Quest data
	The Loader only contains methods, which work on pointers to containers of objects.
*/

class Loader
{
	std::shared_ptr<actions_map> actions;
	std::shared_ptr<Database> db;
public:
	void setActions(std::shared_ptr<actions_map>&);
	void setDB(std::shared_ptr<Database>&);
	void LoadUI(std::string,std::unique_ptr<RenderContext>&);
	void LoadMaps(json, int, std::unique_ptr<RenderContext>&);
};