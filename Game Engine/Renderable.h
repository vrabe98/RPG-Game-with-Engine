#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <chrono>
#include "json.hpp"
#include "RenderContext.h"

using json = nlohmann::json;
using actions_map = std::map<std::string, std::function<int()>>;

class Renderable;
class Texture;
class Button;
class Window;
class Map_view;
class Map;
class Character;
class Main_character;


/*
	Base class for renderable components
*/

class Renderable
{
protected:
	std::unique_ptr<RenderContext>& render_context;
	std::function<int()> action;	//will not be used if the object is not a widget
public:
	virtual bool render()=0;	//bool to return the state of widgets in case it is needed, otherwise required to return false
	virtual ~Renderable() {};
	int act();
	Renderable(std::unique_ptr<RenderContext>&);
};

/*
	Wrapper for hardware-accelerated SDL2 textures
*/

class Texture :public Renderable {
protected:
	SDL_Rect rnd_rect;
	SDL_Texture* texture;
public:
	bool render() final;
	SDL_Texture* get_texture_ptr();
	Texture(json, std::unique_ptr<RenderContext>&);
	Texture(std::unique_ptr<RenderContext>&);
	void create_empty(SDL_Rect);
	SDL_Texture* read_from_file(std::string);
	~Texture();
};

/*
	Wrapper for the textures used to draw characters or objects,
	with extra functionalities
	CURRENTLY JUST A TEXTURE, EXTRA FUNCTIONALITIES WILL COME LATER
*/

class Sprite :public Texture {
public:
	void update_rnd_rect(SDL_Rect);
	Sprite(std::unique_ptr<RenderContext>&);
	Sprite(json, std::unique_ptr<RenderContext>&);
};

/*
	Wrapper classes for Dear ImGUI widgets, derived from Renderable
*/

class Button:public Renderable {
	ImVec2 dim, pos;
	ImVec4 color;
	ImVec4 color_hover, color_active;
	std::string text;
	float round_radius;
public:
	Button(json,std::unique_ptr<RenderContext>&,std::shared_ptr<actions_map>&);
	bool render() override;
};

/*
	The window wraps around multiple widgets
*/

class Window:public Renderable{
	ImVec2 dim, pos;
	std::string name;
	std::vector<std::shared_ptr<Renderable>> renderables;
public:
	Window(json, std::unique_ptr<RenderContext>&, std::shared_ptr<actions_map>&);
	bool render() override;
};

/*
	Container class for the map viewport, used in drawing the current map
*/

class Map_view :public Renderable {
	SDL_Rect viewport;
	std::unique_ptr<std::vector<std::shared_ptr<Character>>>& npcs;
	std::shared_ptr<Main_character> main_char;
	std::shared_ptr<Map>& current_map;	//initialized through the constructor, resides in the database class
	std::unique_ptr<std::vector<std::shared_ptr<Map>>>& maps;
public:
	Map_view(json, std::shared_ptr<Map>&, std::unique_ptr<std::vector<std::shared_ptr<Map>>>&, 
		std::unique_ptr<std::vector<std::shared_ptr<Character>>>&,std::shared_ptr<Main_character>,std::unique_ptr<RenderContext>&);
	bool render() override;
};

/*
	The map:

	-Is a renderable and the render method has to return false
	-Composed of individual rectangular tiles
	-Contains an integer tilemap that determines the terrain type of the tile
	-The tilemap is allocated dinamically and is treated as a 2D matrix
	-The tiles can be rendered at a different size than the source tileset
	-The tiles have associated numbers, from 0 to the number of tiles in the tileset, minus 1
	-THE TILES IN THE TILESET HAVE TO BE ARRANGED HORIZONTALLY AND MUST HAVE THE SAME SIZE
	-The map is pre-rendered on a texture at loading, might change later to loading on demand
	-The map is rendered on a viewport, defined in the screen UI files
	-Maps are also given actual names (used for the data files, for clarity), and have an ID and a description
*/

class Map :public Renderable {
	uint16_t id;								//allows more than 256 maps, used in ordering the maps
	std::unique_ptr<std::valarray<uint8_t>> tilemap;
	std::vector<bool> tile_accessib;
	SDL_Rect viewport;
	SDL_Rect num_tiles, dim_tile;
	Texture texture;
	std::string name, description;
	std::shared_ptr<Coordinate> mainchar_pos;	/*the map is rendered based on the main character position
											this implementation is a workaround as the render() method does not accept arguments*/
public:
	void set_pos_ptr(std::shared_ptr<Coordinate>&);
	void set_viewport(SDL_Rect);	//set the viewport from the map view before every render
	bool can_access();
	SDL_Rect get_tile_dim();
	SDL_Rect get_px_size();
	uint16_t get_id();
	bool render() override;
	Map(json, std::unique_ptr<RenderContext>&);
	~Map();
};

/*
	Classes for in-game characters, methods defined in Character.cpp
*/

struct Stats {
	int strength;
	int dexterity;
	int endurance;
	int intelligence;
	int luck;
	Stats();
	Stats(json);
};

class Character : public Renderable {
protected:
	int id, map_id;
	std::string name, description;
	Sprite sprite;		//will change the sprite system later, to allow characters to move
	std::shared_ptr<Coordinate> pos_px;
	ImVec2 vel, pos_px_flt;
	Stats stats;
	SDL_Rect dim_sprite;	//valid for all sprites of character, makes sense to put it here
	std::chrono::steady_clock::time_point last_render;
public:
	std::shared_ptr<Coordinate>& get_pos();
	Stats& get_stats();
	int get_current_map_id();
	void set_current_map_id(int);
	void stop_move();
	void init_common(json,SDL_Rect);		//avoids a lot of duplicate code
	void change_map(int, SDL_Rect,Coordinate);
	void move(Coordinate,const float,const float,const float,SDL_Rect);		//a coordinate of 0 indicates deceleration
	void force_move(Coordinate);											//used to get the character back to the old position when it hits a "wall"
	bool render() override;
	Character(json, SDL_Rect, std::unique_ptr<RenderContext>&);
	Character(std::unique_ptr<RenderContext>&);
};

class Main_character : public Character {

public:
	Main_character(json, SDL_Rect, std::unique_ptr<RenderContext>&);
};