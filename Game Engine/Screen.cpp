#include "Screen.h"

void Screen::AddRenderable(Renderable* renderable)
{
	renderables.push_back(renderable);
}

Screen::Screen(std::list<Renderable*> renderables) {
	this->renderables = renderables;
}

Screen::~Screen() {
	for (Renderable* rnd : renderables) {
		delete rnd;
	}
}