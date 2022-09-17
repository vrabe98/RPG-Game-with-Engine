#include "Renderable.h"

Renderable::Renderable(std::unique_ptr<RenderContext>& context):render_context(context) {}

int Renderable::act() {
	if(action) return action();
	return 1;
}

/*
	Button class method definitions
*/

Button::Button(json data, std::unique_ptr<RenderContext>& context, std::shared_ptr<actions_map>& actions):
		Renderable(context) {
	this->dim = ImVec2(data["dim"][0], data["dim"][1]);
	this->pos = ImVec2(data["pos"][0], data["pos"][1]);
	this->color = ImVec4(data["color"][0], data["color"][1], data["color"][2],data["color"][3]);
	this->round_radius = data["round_radius"];
	this->text = data["text"].get<std::string>();
	this->color_hover = ImVec4{ color.x + 0.25f,color.y + 0.25f,color.z + 0.25f,color.w };
	this->color_active = ImVec4{ color.x + 0.2f,color.y + 0.2f,color.z + 0.2f,color.w };

	this->action = (*actions)[data["action"].get<std::string>()];
}

bool Button::render() {
	bool res;
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, round_radius);
	ImGui::PushStyleColor(ImGuiCol_Button, color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color_hover);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, color_active);

	ImGui::SetCursorPos(pos);

	res = ImGui::Button(text.c_str(), dim);

	ImGui::PopStyleColor(3);
	ImGui::PopStyleVar();
	return res;
}

/*
	Window class method definitions
*/

Window::Window(json data, std::unique_ptr<RenderContext>& context, std::shared_ptr<actions_map>& actions):
		Renderable(context) {
	this->dim = ImVec2(data["dim"][0], data["dim"][1]);
	this->pos = ImVec2(data["pos"][0], data["pos"][1]);
	this->name = data["name"].get<std::string>();
	for (auto& wdg:data["widgets"]) {
		if (wdg["type"].get<std::string>() == "button") {
			std::shared_ptr<Button> btn = std::make_shared<Button>(wdg, render_context,actions);
			renderables.push_back(btn);
		}
	}
}

bool Window::render(){
	bool act=true;
	ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
	ImGui::SetWindowPos(pos);
	ImGui::SetWindowSize(dim);
	ImGui::PushFont(render_context->font);

	for (auto& rnd : renderables) {
		if (rnd->render()) {
			act=rnd->act();
		}
	}

	ImGui::PopFont();
	ImGui::End();
	return act;
}

/*
	Texture class method definitions
*/

bool Texture::render(){
	SDL_RenderCopy(render_context->renderer, texture, NULL, &rnd_rect);
	return false;
}

Texture::Texture(json data, std::unique_ptr<RenderContext>& context):Renderable(context) {
	this->rnd_rect = SDL_Rect{ data["pos"][0], data["pos"][1],data["dim"][0], data["dim"][1] };
	texture = nullptr;
	SDL_Surface* surf = IMG_Load(data["path"].get<std::string>().c_str());
	if (surf == nullptr) {
		throw std::runtime_error("Texture " + data["path"].get<std::string>() + " could not be loaded!");
	}
	texture = SDL_CreateTextureFromSurface(render_context->renderer, surf);
	if (texture == nullptr) {
		throw std::runtime_error("Texture " + data["path"].get<std::string>() + " could not be created!");
	}
	SDL_FreeSurface(surf);
}

Texture::~Texture(){
	SDL_DestroyTexture(texture);
}
