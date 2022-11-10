#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
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
	SDL_Rect rnd_rect;
	SDL_Texture* texture;
public:
	bool render() override;
	SDL_Texture* get_texture_ptr();
	Texture(json, std::unique_ptr<RenderContext>&);
	Texture(std::unique_ptr<RenderContext>&);
	void create_empty(SDL_Rect);
	SDL_Texture* read_from_file(std::string);
	~Texture();
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
	std::shared_ptr<Map>& current_map;	//initialized through the constructor, resides in the database class
	std::unique_ptr<std::vector<std::shared_ptr<Map>>>& maps;
public:
	Map_view(json, std::shared_ptr<Map>& current_map, std::unique_ptr<std::vector<std::shared_ptr<Map>>>&,std::unique_ptr<RenderContext>&);
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
	std::valarray<uint8_t> tilemap;
	SDL_Rect viewport;
	SDL_Rect num_tiles, dim_tile, dim_source_tile;
	Texture texture;
	std::string name, description;
	std::shared_ptr<Coordinate> mainchar_pos;	/*the map is rendered based on the main character position
											this implementation is a workaround as the render() method does not accept arguments*/
public:
	void set_pos_ptr(std::shared_ptr<Coordinate>&);
	void set_viewport(SDL_Rect);	//set the viewport from the map view before every render
	bool render() override;
	Map(json, std::unique_ptr<RenderContext>&);
	~Map();
};