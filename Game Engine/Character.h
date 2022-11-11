#pragma once
#include "Renderable.h"

class Character : public Renderable{
	Coordinate pos;
	int map_id;
public:
	bool render() override;
};

class Main_character : public Character {
public:
	bool render() override;
};