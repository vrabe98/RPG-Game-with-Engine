#include "Renderable.h"

Renderable::Renderable(std::unique_ptr<RenderContext>& context):render_context(context) {}

int sgn(float x) {
	if (x < 0) return -1;
	else if (x > 0) return 1;
	return 0;
}

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
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
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
	Sprite class method definitions
*/

bool Sprite::render() {
	if(flip) SDL_RenderCopyEx(render_context->renderer, texture, NULL, &rnd_rect,0.0,NULL,SDL_FLIP_HORIZONTAL);
	else SDL_RenderCopy(render_context->renderer, texture, NULL, &rnd_rect);
	flip = false;
	return false;
}

void Sprite::update_rnd_rect(SDL_Rect rnd_rect) {
	this->rnd_rect = rnd_rect;
}

int Sprite::get_num_frames() {
	return num_frames;
}

void Sprite::set_num_frames(int num_frames) {
	this->num_frames = num_frames;
}

void Sprite::flip_sprite() {
	flip = true;
}

Sprite::Sprite(json data, std::unique_ptr<RenderContext>& context):Texture(context) {
	num_frames = 0;
	flip = false;
}

Sprite::Sprite(std::unique_ptr<RenderContext>& context) :Texture(context) {
	num_frames = 0;
	flip = false;
}

/*
	Map view method definitions
*/

Map_view::Map_view(json data, std::shared_ptr<Map>& current_map,
		std::unique_ptr<std::vector<std::shared_ptr<Map>>>& maps,
		std::unique_ptr<std::vector<std::shared_ptr<Character>>>& npcs,
		std::shared_ptr<Main_character> main_char,
		std::unique_ptr<RenderContext>& context):
		Renderable(context),current_map(current_map),maps(maps),npcs(npcs) {
	this->main_char = main_char;
	this->viewport = SDL_Rect{ data["pos"][0], data["pos"][1],data["dim"][0], data["dim"][1] };
}

bool Map_view::render() {
	SDL_Rect src = { 0,0,0,0 };
	SDL_Rect map_dim = current_map->get_px_size();
	std::shared_ptr<Coordinate>& mainchar_pos = main_char->get_pos();

	//Render the main character in the middle if possible
	src.w = std::min(viewport.w, map_dim.w);
	src.h = std::min(viewport.h, map_dim.h);
	src.x = mainchar_pos->x-src.w/2;
	src.y = mainchar_pos->y-src.h/2;

	//Character too close to the map margins
	if (src.x < 0) src.x = 0;
	else if (src.x + src.w > map_dim.w) src.x = map_dim.w - src.w;
	if (src.y < 0) src.y = 0;
	else if (src.y + src.h > map_dim.h) src.y = map_dim.h - src.h;

	SDL_SetRenderTarget(render_context->renderer, render_context->canvas);
	current_map->render();
	main_char->render();
	for (auto& npc : *npcs) {
		if (npc->get_current_map_id() == current_map->get_id()) {
			npc->render();
		}
	}
	SDL_RenderSetViewport(render_context->renderer, &viewport);
	SDL_SetRenderTarget(render_context->renderer, NULL);
	SDL_RenderCopy(render_context->renderer, render_context->canvas, &src, NULL);
	return false;
}

/*
	Map method declarations
*/

void Map::set_pos_ptr(std::shared_ptr<Coordinate>& ptr) {
	mainchar_pos = ptr;
}

bool Map::can_access() {
	Coordinate pos = *mainchar_pos;
	pos.x = pos.x / dim_tile.w;
	pos.y = pos.y / dim_tile.h;
	if (pos.x > (num_tiles.w-1) || pos.y > (num_tiles.h-1)) return false;
	return tile_accessib[(*tilemap)[pos.y * num_tiles.w + pos.x]];
}

SDL_Rect Map::get_tile_dim() {
	return dim_tile;
}

SDL_Rect Map::get_px_size() {
	return SDL_Rect{ 0,0,num_tiles.w * dim_tile.w,num_tiles.h * dim_tile.h };
}

/*Renders in the viewport's upper left corner, whatever fits from the map
or the whole map along the dimensions smaller than the viewport size
*/
bool Map::render() {
	SDL_Rect canvas_size = { 0,0,0,0 };
	if(render_context->canvas!=nullptr) SDL_QueryTexture(render_context->canvas, NULL, NULL, &canvas_size.w, &canvas_size.h);
	if (render_context->canvas == nullptr || map_dim_pixels.w != canvas_size.w || map_dim_pixels.h != canvas_size.h) {
		if (render_context->canvas != nullptr) SDL_DestroyTexture(render_context->canvas);
		render_context->create_empty_canvas(map_dim_pixels);
	}
	SDL_RenderCopy(render_context->renderer, texture.get_texture_ptr(), NULL,NULL);
	return false;
}

uint16_t Map::get_id() {
	return id;
}

Map::Map(json metadata, std::unique_ptr<RenderContext>& context) :Renderable(context),texture(context) {
	SDL_Rect dim_source_tile;
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
	tilemap = std::make_unique<std::valarray<uint8_t>>();
	tilemap->resize(num_tiles.w * num_tiles.h);
	while (cnt<tilemap->size()) {
		tilemap_file >> tmp;
		(*tilemap)[cnt++]=(uint8_t)tmp;
	}

	//Read the tileset and render to the map texture
	Texture tileset(context);
	SDL_Texture* tileset_texture=tileset.read_from_file(metadata["tileset_path"].get<std::string>());

	texture.create_empty(map_dim_pixels);
	tex = texture.get_texture_ptr();	//working directly with the SDL texture
	SDL_SetRenderTarget(context->renderer, tex);

	for (int i = 0; i < num_tiles.h;i++) {
		for (int j = 0; j < num_tiles.w;j++) {
			SDL_Rect src = { (*tilemap)[i * num_tiles.w + j] * dim_source_tile.w,0,dim_source_tile.w,dim_source_tile.h };
			SDL_Rect dest = { j * dim_tile.w,i * dim_tile.h,dim_tile.w,dim_tile.h };
			SDL_RenderCopy(render_context->renderer, tileset_texture, &src, &dest);
		}
	}

	//read tile accessibility data
	for (auto& tile_acc : metadata["tile_accessibility"]) tile_accessib.push_back(tile_acc);

	SDL_DestroyTexture(tileset_texture);
	SDL_SetRenderTarget(context->renderer, NULL);
}

Map::~Map() {
	mainchar_pos.reset();
}

/*
	Method definitions for the character classes
*/

Stats::Stats(json data) {
	strength = data["strength"];
	dexterity = data["dexterity"];
	endurance = data["endurance"];
	intelligence = data["intelligence"];
	luck = data["luck"];
}

Stats::Stats() {
	strength = dexterity = endurance = intelligence = luck = 0;
}

std::shared_ptr<Coordinate>& Character::get_pos() {
	return pos_px;
}

int Character::get_current_map_id() {
	return map_id;
}

void Character::set_current_map_id(uint16_t map_id) {
	this->map_id = map_id;
}

void Character::set_map_orig(Coordinate orig) {
	this->map_orig = orig;
}

void Character::move(Coordinate dir,const float accel,const float decel,const float max_vel,SDL_Rect map_size) {
	if (dir.x != 0 || dir.y != 0 || vel.x != 0 || vel.y != 0) {				//only update position when needed
		ImVec2 newpos, oldvel = vel;

		if (dir.x != 0) vel.x += (float)dir.x * accel * render_context->elapsed.count();	//update velocity
		else vel.x = vel.x - sgn(vel.x) * decel * render_context->elapsed.count();

		if (sgn(oldvel.x)!=0&&sgn(vel.x)!=sgn(oldvel.x)) vel.x = 0.0;		//clamp velocity
		else if (sgn(vel.x) * vel.x >= max_vel) vel.x = sgn(vel.x) * max_vel;

		if (dir.y != 0) vel.y += (float)dir.y * accel * render_context->elapsed.count();	//update velocity
		else vel.y = vel.y - sgn(vel.y) * decel * render_context->elapsed.count();

		if (sgn(oldvel.y)&&sgn(vel.y)!=sgn(oldvel.y)) vel.y = 0.0;			//clamp velocity
		else if (sgn(vel.y) * vel.y >= max_vel) vel.y = sgn(vel.y) * max_vel;

		newpos = { pos_px_flt.x + vel.x*render_context->elapsed.count(),pos_px_flt.y + vel.y*render_context->elapsed.count()};

		if (newpos.x >= 0.0 && newpos.x <= (float)map_size.w) pos_px_flt.x = newpos.x;
		else {
			vel.x = 0.0;				//stop the character
			if (newpos.x < 0.0) pos_px_flt.x = 0.0;
			else if (newpos.x > map_size.x) pos_px_flt.x = (float)map_size.w;
		}
		if (newpos.y >= 0.0 && newpos.y <= (float)map_size.w) pos_px_flt.y = newpos.y;
		else {
			vel.y = 0.0;				//stop the character
			if (newpos.y < 0) pos_px_flt.y = 0.0;
			else if (newpos.y > map_size.y) pos_px_flt.y = (float)map_size.w;
		}
		pos_px->x = (int)pos_px_flt.x;
		pos_px->y = (int)pos_px_flt.y;
	}
}

void Character::set_state(int state) {
	this->state = state;
	sprite_cnt = 0;
}

void Character::stop_move() {
	vel = ImVec2{ 0.0f,0.0f };
}

Stats& Character::get_stats() {
	return stats;
}

void Character::force_move(Coordinate pos) {
	*pos_px = pos;
	pos_px_flt.x = (float)pos.x;
	pos_px_flt.y = (float)pos.y;
}

bool Character::render() {
	int old_state = state;
	if (vel.x > 0) state = WALK_E;
	else if (vel.x < 0) state = WALK_W;
	else if (vel.y > 0) state = WALK_S;
	else if (vel.y < 0) state = WALK_N;
	else state = IDLE;
	if (sprites[state].isempty()&&(state==WALK_N||state==WALK_S||state==WALK_E||state==WALK_W)) state = WALK_GENERIC;

	if (state != old_state) {
		sprite_cnt = 0;	//reset animation
		sprite = sprites[state].get_last()->get_next();		//get first element of the list
	}
	else if (++sprite_cnt >= sprite->get_data()->get_num_frames()) {		//progress the animation
		sprite_cnt = 0;
		sprite = sprite->get_next();
	}
	sprite->get_data()->update_rnd_rect(SDL_Rect{ pos_px->x - dim_sprite.w / 2,pos_px->y - dim_sprite.h,dim_sprite.w,dim_sprite.h });
	if (state == WALK_GENERIC&&vel.x>0) sprite->get_data()->flip_sprite();
	sprite->get_data()->render();
	return false;
}

void Character::init_common(json data,SDL_Rect map_tile_size) {
	SDL_Rect dim_source_sprite = SDL_Rect{ 0,0,data["dim_source_sprite"][0],data["dim_source_sprite"][1] };
	Texture spritesheet(render_context);
	SDL_Texture* spritesheet_texture;
	id = data["ID"];
	map_id = data["map_id"];
	stats = Stats(data["stats"]);
	vel = { 0.0,0.0 };

	state = IDLE;
	sprite_cnt = 0;
	sprites.resize(MIN_STATES);

	pos_tiles = Coordinate(data["pos"][0], data["pos"][1]);
	pos_px = std::make_shared<Coordinate>(pos_tiles.x * map_tile_size.w, pos_tiles.y * map_tile_size.h);
	pos_px_flt = { (float)pos_px->x,(float)pos_px->y };
	dim_sprite = SDL_Rect{ 0,0,data["dim_sprite"][0],data["dim_sprite"][1] };

	spritesheet_texture = spritesheet.read_from_file(data["spritesheet_path"].get<std::string>());

	init_sprites(data["sprite_data"], spritesheet_texture, dim_sprite, dim_source_sprite);
	SDL_DestroyTexture(spritesheet_texture);

	SDL_SetRenderTarget(render_context->renderer, NULL);
}

void Character::init_sprites(json data,SDL_Texture* spritesheet,SDL_Rect dim_sprite,SDL_Rect dim_source_sprite) {
	for (auto& animation : data) {
		int type = -1;
		std::string type_name = animation["state"].get<std::string>();
		//convert the string to enum, can't find another more elegant way
		if (type_name == "IDLE") type = IDLE;
		else if (type_name == "WALK_W") type = WALK_W;
		else if (type_name == "WALK_E") type = WALK_E;
		else if (type_name == "WALK_N") type = WALK_N;
		else if (type_name == "WALK_S") type = WALK_S;
		else if (type_name == "WALK_GENERIC") type = WALK_GENERIC;

		if (type != -1) {
			for (int i = 0; i < animation["num_sprites"]; i++) {
				std::shared_ptr<Sprite> new_sprite = std::make_shared<Sprite>(render_context);
				SDL_Texture* texture;
				new_sprite->create_empty(dim_sprite);
				dim_source_sprite.x = i * dim_source_sprite.w;			//reuse the variable to define the clipped sprite coordinates
				dim_source_sprite.y = type * dim_source_sprite.h;
				texture = new_sprite->get_texture_ptr();
				SDL_SetRenderTarget(render_context->renderer, texture);
				SDL_RenderCopy(render_context->renderer, spritesheet, &dim_source_sprite, NULL);
				new_sprite->set_num_frames(animation["frames"]);
				sprites[type].add_back(new_sprite);
			}
		}
		else {
			throw std::runtime_error("Error reading sprite for "+name+"!");
		}
	}
	sprite = sprites[state].get_last()->get_next();
}

Character::Character(json data, SDL_Rect map_tile_size,std::unique_ptr<RenderContext>& context) :Renderable(context) {
	init_common(data, map_tile_size);
	name = data["name"].get<std::string>();
	description = data["description"].get<std::string>();
}

Character::Character(std::unique_ptr<RenderContext>& context) :Renderable(context){
	
}

Main_character::Main_character(json data, SDL_Rect map_tile_size,std::unique_ptr<RenderContext>& context):Character(context) {
	init_common(data,map_tile_size);
}