#pragma once
#include <Game.h>

const float char_accel = 0.003f, char_decel = 0.0015f, max_vel = 0.055f;
/*
	Custom Game class that provides an implementation of the Play() method.
*/

class CustomGame:public Game
{
public:
	CustomGame(const char* name) :Game(name) {}
	void Play() override;
	void moveMain(Coordinate);
};

extern CustomGame game;
extern std::shared_ptr<Database> db;