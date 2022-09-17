#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "json.hpp"
#include "RenderContext.h"

using json = nlohmann::json;
using actions_map = std::map<std::string, std::function<int()>>;

/*
	Base class for renderable components
*/

class Renderable
{
protected:
	std::unique_ptr<RenderContext>& render_context;
	std::function<int()> action;	//will not be used if the object is not a widget
public:
	virtual bool render()=0;	//bool to return the state of widgets in case it is needed, otherwise required to return false
	virtual ~Renderable() {};
	int act();
	Renderable(std::unique_ptr<RenderContext>&);
};

/*
	Wrapper for hardware-accelerated SDL2 textures
*/

class Texture :public Renderable {
	SDL_Rect rnd_rect;
	SDL_Texture* texture;
public:
	bool render();
	Texture(json, std::unique_ptr<RenderContext>&);
	~Texture();
};

/*
	Wrapper classes for Dear ImGUI widgets, derived from Renderable
*/

class Button:public Renderable {
	ImVec2 dim, pos;
	ImVec4 color;
	ImVec4 color_hover, color_active;
	std::string text;
	float round_radius;
public:
	Button(json,std::unique_ptr<RenderContext>&,std::shared_ptr<actions_map>&);
	bool render();
};

/*
	The window wraps around multiple widgets
*/

class Window:public Renderable{
	ImVec2 dim, pos;
	std::string name;
	std::vector<std::shared_ptr<Renderable>> renderables;
public:
	Window(json, std::unique_ptr<RenderContext>&, std::shared_ptr<actions_map>&);
	bool render();
};