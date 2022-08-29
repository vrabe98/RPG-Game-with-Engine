#pragma once
#include "Renderable.h"

class Game
{
	RenderContext render_context;
	std::string name;
public:
	RenderContext* getRenderContext() {
		return &render_context;
	}
	void Init_SDL2_ImGUI();
	void Shutdown();
	Game(std::string);
};

