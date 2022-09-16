#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include "Screen.h"

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
	std::shared_ptr<RenderContext> render_context;
	std::string name;
public:
	std::shared_ptr<RenderContext> getRenderContext();
	void Init_SDL2_ImGUI();
	void Load(std::string data_paths_json);
	void Shutdown();
	void setActions(std::shared_ptr<actions_map>);
	void setDB(std::shared_ptr<Database>);
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
	std::vector<std::shared_ptr<Screen>> screens;
public:
	Database(){}
	std::shared_ptr<Screen> getScreen(int);
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
	void setActions(std::shared_ptr<actions_map>);
	void setDB(std::shared_ptr<Database>);
	void LoadUI(std::string,std::shared_ptr<RenderContext>);
};