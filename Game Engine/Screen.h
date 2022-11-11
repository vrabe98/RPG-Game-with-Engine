#pragma once
#include <list>
#include "Character.h"	//includes Renderable.h

/*
	The Screen class is basically a container for various renderable objects.
*/

class Screen
{
	std::string name;
	std::unique_ptr<RenderContext>& render_context;
	std::vector<std::shared_ptr<Window>> windows;				//dear ImGUI windows
	std::vector<std::shared_ptr<Renderable>> renderables;		//misc graphics (background, the game map, etc.)
public:
	void AddWindow(std::shared_ptr<Window>);
	void AddRenderable(std::shared_ptr<Renderable>);
	bool Render();
	Screen(std::unique_ptr<RenderContext>&);
	Screen(std::vector<std::shared_ptr<Window>>, std::vector<std::shared_ptr<Renderable>>, std::unique_ptr<RenderContext>&);
	~Screen();
};

