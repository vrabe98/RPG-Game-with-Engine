#include "Renderable.h"

Renderable::Renderable() {
	render_context = nullptr;
}

Renderable::Renderable(std::shared_ptr<RenderContext> context) {
	this->render_context = context;
}

/*
	Button class method definitions
*/

Button::Button(json data, std::shared_ptr<RenderContext> context):Renderable(context) {
	this->dim = ImVec2(data["dim"][0], data["dim"][1]);
	this->pos = ImVec2(data["pos"][0], data["pos"][1]);
	this->color = ImVec4(data["color"][0], data["color"][1], data["color"][2],data["color"][3]);
	this->round_radius = data["round_radius"];
	this->text = data["text"].get<std::string>();
	this->color_hover = ImVec4{ color.x + 0.25f,color.y + 0.25f,color.z + 0.25f,color.w };
	this->color_active = ImVec4{ color.x + 0.2f,color.y + 0.2f,color.z + 0.2f,color.w };

	this->action = data["action"].get<std::string>();
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

Window::Window(json data, std::shared_ptr<RenderContext> context):Renderable(context) {
	this->dim = ImVec2(data["dim"][0], data["dim"][1]);
	this->pos = ImVec2(data["pos"][0], data["pos"][1]);
	this->name = data["name"].get<std::string>();
	for (auto& wdg:data["widgets"]) {
		if (wdg["type"].get<std::string>() == "button") {
			std::shared_ptr<Button> btn = std::make_shared<Button>(wdg, render_context);
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
			if (rnd->getAction() == "exit") {
				act = false;
			}
		}
	}

	ImGui::PopFont();
	ImGui::End();
	return act;
}