#pragma once
#include "Renderable.h"

/*
	Abstract class with a pure virtual method. This method's implementation has to be provided by the user,
	and it's role will be to host the main game loop.
*/

class Game
{
protected:
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