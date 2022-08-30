#pragma once
#include <string>
#include "RenderContext.h"

/*
	Base class for renderable components
*/

class Renderable
{
protected:
	RenderContext* render_context;
public:
	virtual bool render()=0;	//bool to return the state of widgets in case it is needed, otherwise required to return true
	Renderable() { render_context = nullptr; }
	Renderable(RenderContext* context) {
		this->render_context = context;
	}
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
	Button(const char* text, RenderContext* context, const ImVec2& dim, const ImVec2& pos, const ImVec4& color, float round_radius=0.0f);
	bool render();
};
