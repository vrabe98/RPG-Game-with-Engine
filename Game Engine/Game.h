#pragma once
#include "Screen.h"
class Loader;

/*
	Abstract class with a pure virtual method. This method's implementation has to be provided by the user,
	and it's role will be to host the main game loop.
*/

class Game
{
protected:
	Loader* loader;
	std::list<Screen> screens;
	RenderContext render_context;
	std::string name;
public:
	RenderContext* getRenderContext() {
		return &render_context;
	}
	void Init_SDL2_ImGUI();
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
	void LoadUI(std::string ui_folderpath, std::list<Screen>& screens);
};