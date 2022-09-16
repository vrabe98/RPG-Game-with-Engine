#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include <SDL.h>
#include <SDL_image.h>
#include <stdexcept>

#define WIN_W 1280
#define WIN_H 720

struct RenderContext
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	ImFont* font;
	RenderContext(std::string);
	~RenderContext();
};

