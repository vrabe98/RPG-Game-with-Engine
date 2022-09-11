#pragma once
#include <list>
#include "Renderable.h"

/*
	The Screen class is basically a container for various renderable objects.
*/

class Screen
{
	std::string name;
	RenderContext* render_context;
	std::list<Window*> windows;				//dear ImGUI windows
	std::list<Renderable*> renderables;		//misc graphics (background, the game map, etc.)
public:
	void AddWindow(Window*);
	void AddRenderable(Renderable*);
	bool Render();
	Screen(RenderContext*);
	Screen(std::list<Window*>,RenderContext*);
	~Screen();
};

