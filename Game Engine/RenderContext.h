#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include <SDL.h>
#include <stdexcept>

#define WIN_W 1280
#define WIN_H 720

struct RenderContext
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	ImFont* font;
	void Init(std::string);
	void Destroy();
	RenderContext() {}
	RenderContext(SDL_Window*, SDL_Renderer*, ImFont*);
};

