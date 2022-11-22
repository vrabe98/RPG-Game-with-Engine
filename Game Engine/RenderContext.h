#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include <SDL.h>
#include <SDL_image.h>
#include <stdexcept>
#include <chrono>
#include <ratio>

#define WIN_W 1280
#define WIN_H 720

struct RenderContext
{
	SDL_Window* window;
	SDL_Renderer* renderer;
	ImFont* font;
	SDL_Texture* canvas;
	SDL_Rect* map_tile_size;
	std::chrono::duration<int64_t,std::milli> elapsed;
	void create_empty_canvas(SDL_Rect);
	RenderContext(std::string);
	~RenderContext();
};

//use this instead of ImVec2 whenever int coordinates are needed
struct Coordinate {
	int x, y;
};