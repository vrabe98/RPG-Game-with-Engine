#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "json.hpp"
#include "RenderContext.h"

using json = nlohmann::json;

/*
	Base class for renderable components
*/

class Renderable
{
protected:
	std::shared_ptr<RenderContext> render_context;
	std::string action;			//PLACEHOLDER FOR QUICK TESTING
public:
	virtual bool render()=0;	//bool to return the state of widgets in case it is needed, otherwise required to return true
	virtual ~Renderable() {}
	std::string getAction() { return action; }	//PLACEHOLDER FOR QUICK TESTING
	Renderable();
	Renderable(std::shared_ptr<RenderContext>);
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
	Button() {};
	Button(json,std::shared_ptr<RenderContext>);
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
	Window() {};
	Window(json, std::shared_ptr<RenderContext>);
	bool render();
};