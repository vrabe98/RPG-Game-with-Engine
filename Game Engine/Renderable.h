#pragma once
#include <string>
#include "RenderContext.h"

class Renderable				//abstract class that all renderable classes inherit
{
protected:
	RenderContext* render_context;
public:
	virtual bool render()=0;	//bool to return the state of widgets in case it is needed, otherwise will return true
	Renderable() { render_context = nullptr; }
	Renderable(RenderContext* context) {
		this->render_context = context;
	}
};

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
