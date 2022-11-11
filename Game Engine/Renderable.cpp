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

void Texture::create_empty(SDL_Rect dim) {
	this->rnd_rect = dim;
	texture = SDL_CreateTexture(render_context->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, dim.w, dim.h);
	if (texture == nullptr) {
		throw std::runtime_error(SDL_GetError());
	}
}

SDL_Texture* Texture::get_texture_ptr() {
	return texture;
}

SDL_Texture* Texture::read_from_file(std::string path) {
	texture = nullptr;
	SDL_Surface* surf = IMG_Load(path.c_str());
	if (surf == nullptr) {
		throw std::runtime_error("Texture " + path + " could not be loaded!");
	}
	texture = SDL_CreateTextureFromSurface(render_context->renderer, surf);
	if (texture == nullptr) {
		throw std::runtime_error("Texture " + path + " could not be created!");
	}
	SDL_FreeSurface(surf);
	return texture;
}

Texture::Texture(json data, std::unique_ptr<RenderContext>& context) :Renderable(context) {
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

Texture::Texture(std::unique_ptr<RenderContext>& context) :Renderable(context) {
	this->rnd_rect = { 0,0,0,0 };
	texture = nullptr;
}

Texture::~Texture(){
	SDL_DestroyTexture(texture);
}

/*
	Map view method definitions
*/

Map_view::Map_view(json data, std::shared_ptr<Map>& current_map,
		std::unique_ptr<std::vector<std::shared_ptr<Map>>>& maps,std::unique_ptr<RenderContext>& context):
		Renderable(context),current_map(current_map),maps(maps) {
	this->viewport = SDL_Rect{ data["pos"][0], data["pos"][1],data["dim"][0], data["dim"][1] };

}

bool Map_view::render() {
	current_map->set_viewport(viewport);
	current_map->render();
	return false;
}

/*
	Map method declarations
*/

void Map::set_pos_ptr(std::shared_ptr<Coordinate>& ptr) {
	mainchar_pos = ptr;
}

void Map::set_viewport(SDL_Rect viewport) {
	this->viewport = viewport;
}

/*Renders in the viewport's upper left corner, whatever fits from the map
or the whole map along the dimensions smaller than the viewport size
I HAVE TO MODIFY THE DEPENDENCE ON THE CHARACTER'S POSITION!
*/
bool Map::render() {
	SDL_Rect src = {mainchar_pos->x,mainchar_pos->y,0,0};
	SDL_Rect dest = { 0,0,0,0 };

	src.w = std::min(num_tiles.w * dim_tile.w, viewport.w);
	src.h = std::min(num_tiles.h * dim_tile.h, viewport.h);

	dest.w = src.w;
	dest.h = src.h;

	SDL_RenderSetViewport(render_context->renderer, &viewport);
	SDL_RenderCopy(render_context->renderer, texture.get_texture_ptr(), &src,&dest);
	return false;
}

uint16_t Map::get_id() {
	return id;
}

Map::Map(json metadata, std::unique_ptr<RenderContext>& context) :Renderable(context),texture(context) {
	SDL_Rect map_dim_pixels;
	SDL_Texture* tex;
	std::ifstream tilemap_file(metadata["tilemap_path"].get<std::string>());
	id = metadata["ID"];
	name = metadata["name"].get<std::string>();
	description = metadata["description"].get<std::string>();

	//x and y unused, only w and h
	num_tiles = SDL_Rect{0,0, metadata["num_tiles"][0],metadata["num_tiles"][1]};
	dim_tile = SDL_Rect{0,0,metadata["dim_tile"][0],metadata["dim_tile"][1] };
	dim_source_tile = SDL_Rect{0,0, metadata["dim_source_tile"][0],metadata["dim_source_tile"][1] };

	map_dim_pixels = SDL_Rect{ 0,0,num_tiles.w * dim_tile.w,num_tiles.h * dim_tile.h };

	//Read the tilemap
	int cnt = 0, tmp;
	tilemap.resize(num_tiles.w * num_tiles.h);
	while (cnt<tilemap.size()) {
		tilemap_file >> tmp;
		tilemap[cnt++]=(uint8_t)tmp;
	}

	//Read the tileset and render to the map texture
	Texture tileset(context);
	SDL_Texture* tileset_texture=tileset.read_from_file(metadata["tileset_path"].get<std::string>());

	texture.create_empty(map_dim_pixels);
	tex = texture.get_texture_ptr();	//working directly with the SDL texture
	SDL_SetRenderTarget(context->renderer, tex);

	for (int i = 0; i < num_tiles.h;i++) {
		for (int j = 0; j < num_tiles.w;j++) {
			SDL_Rect src = { tilemap[i * num_tiles.w + j] * dim_source_tile.w,0,dim_source_tile.w,dim_source_tile.h };
			SDL_Rect dest = { j * dim_tile.w,i * dim_tile.h,dim_tile.w,dim_tile.h };
			SDL_RenderCopy(render_context->renderer, tileset_texture, &src, &dest);
		}
	}

	SDL_SetRenderTarget(context->renderer, NULL);
}

Map::~Map() {
	mainchar_pos.reset();
}
