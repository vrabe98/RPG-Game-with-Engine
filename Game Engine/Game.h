#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include "Screen.h"

class Loader;

/*
	Abstract class with a pure virtual method. This method's implementation has to be provided by the user,
	and it's role will be to host the main game loop.
*/

class Game
{
protected:
	std::unique_ptr<Loader> loader;							//can theoretically be destroyed after loading
	std::vector<std::shared_ptr<Screen>> screens;
	std::shared_ptr<RenderContext> render_context;
	std::string name;
public:
	std::shared_ptr<RenderContext> getRenderContext();
	void Init_SDL2_ImGUI();
	void Load(std::string data_paths_json);
	void Shutdown();
	virtual void Play() = 0;
	Game(std::string);
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
public:
	void LoadUI(std::string, std::vector<std::shared_ptr<Screen>>&,std::shared_ptr<RenderContext>);
};