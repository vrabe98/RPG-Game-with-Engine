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
	std::list<Window*> windows;
public:
	void AddWindow(Window*);
	bool Render();
	Screen(RenderContext* context) { this->render_context = context; }
	Screen(std::list<Window*>,RenderContext*);
	~Screen();
};

