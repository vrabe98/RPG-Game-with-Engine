#include "Actions.h"

/*
	All action methods defined here can access game and db methods, though db should be preferred
*/

int Exit() {
	return 0;
}

int Play() {
	game.setState(CHAR_CREATION);
	return 1;
}

std::shared_ptr<actions_map> actions = std::make_shared<actions_map>(
	std::initializer_list<actions_map::value_type>{
		{"exit", std::bind(Exit)},
		{"play", std::bind(Play)}
	}
);