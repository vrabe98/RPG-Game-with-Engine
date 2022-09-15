#include "Screen.h"

void Screen::AddWindow(std::shared_ptr<Window> window){
	windows.push_back(window);
}

void Screen::AddRenderable(std::shared_ptr<Renderable> rnd){
	renderables.push_back(rnd);
}

bool Screen::Render(){
	bool act = true;
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	for (std::shared_ptr<Window> wnd : windows) {
		if(!wnd->render()) act=false;
	}

	for (std::shared_ptr<Renderable> rnd : renderables) {
		rnd->render();
	}

	ImGui::Render();
	SDL_SetRenderDrawColor(render_context->renderer, (Uint8)0, (Uint8)0, (Uint8)0, (Uint8)1);
	SDL_RenderClear(render_context->renderer);
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(render_context->renderer);
	return act;
}

Screen::Screen(std::shared_ptr<RenderContext> context){
	this->render_context = context;
}

Screen::Screen(std::vector<std::shared_ptr<Window>> wnds, std::vector<std::shared_ptr<Renderable>> renderables,
		std::shared_ptr<RenderContext> context) {
	this->windows = wnds;
	this->renderables = renderables;
	this->render_context = context;
}

Screen::~Screen() {
	for (std::shared_ptr<Window> wnd : windows) {
		wnd.reset();
	}
	for (std::shared_ptr<Renderable> rnd : renderables) {
		rnd.reset();
	}
}