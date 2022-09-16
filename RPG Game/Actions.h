#pragma once
#include "CustomGame.h"

enum States {
	MAIN_MENU,
	CHAR_CREATION
};

extern std::shared_ptr<actions_map> actions;

//The functions return 0 when the game will be closed, maybe other uses can be added

int Exit();
int Play();