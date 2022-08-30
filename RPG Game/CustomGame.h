#pragma once
#include <Game.h>

/*
	Custom Game class that provides an implementation of the Play() method.
*/

class CustomGame:public Game
{
public:
	CustomGame(const char* name) :Game(name) {}
	void Play() override;
};

