#include "Screen.h"

void Screen::AddWindow(Window* window){
	windows.push_back(window);
}

bool Screen::Render(){
	bool act = true;
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	for (Window* wnd : windows) {
		if(!wnd->render()) act=false;
	}

	ImGui::Render();
	SDL_SetRenderDrawColor(render_context->renderer, (Uint8)0, (Uint8)0, (Uint8)0, (Uint8)1);
	SDL_RenderClear(render_context->renderer);
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(render_context->renderer);
	return act;
}

Screen::Screen(std::list<Window*> wnds,RenderContext* context) {
	this->windows = wnds;
	this->render_context = context;
}

Screen::~Screen() {
	for (Window* wnd : windows) {
		delete wnd;
	}
}