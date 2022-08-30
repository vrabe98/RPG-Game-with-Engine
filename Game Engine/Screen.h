#pragma once
#include <list>
#include "Renderable.h"

/*
	The Screen class is basically a container for various renderable objects.
*/

class Screen
{
	std::string name;
	std::list<Renderable*> renderables;
public:
	void AddRenderable(Renderable*);
	Screen(){}
	Screen(std::list<Renderable*>);
	~Screen();
};

