#include "Screen.h"

void Screen::AddWindow(std::shared_ptr<Window> window){
	windows.push_back(window);
}

void Screen::AddRenderable(std::shared_ptr<Renderable> rnd){
	renderables.push_back(rnd);
}

bool Screen::Render(){
	bool act = true;
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	for (auto& wnd : windows) {
		if (!wnd->render()) act = false;
	}

	ImGui::Render();
	SDL_SetRenderDrawColor(render_context->renderer, 0x00,0x00,0x00,0xff);
	SDL_RenderClear(render_context->renderer);

	for (auto& rnd : renderables) {
		rnd->render();
	}

	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(),render_context->renderer);
	SDL_RenderPresent(render_context->renderer);
	return act;
}

Screen::Screen(std::unique_ptr<RenderContext>& context):render_context(context) {}

Screen::Screen(std::vector<std::shared_ptr<Window>> wnds, std::vector<std::shared_ptr<Renderable>> renderables,
		std::unique_ptr<RenderContext>& context):render_context(context) {
	this->windows = wnds;
	this->renderables = renderables;
}

Screen::~Screen() {
	for (auto& wnd : windows) {
		wnd.reset();
	}
	for (auto& rnd : renderables) {
		rnd.reset();
	}
}