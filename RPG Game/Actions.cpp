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

int Back_to_start() {
	game.setState(MAIN_MENU);
	return 1;
}

int Inc_map() {
	int current_map = db->get_current_map_nr();
	db->update_current_map(++current_map);
	return 1;
}

int Dec_map() {
	int current_map = db->get_current_map_nr();
	db->update_current_map(--current_map);
	return 1;
}

std::shared_ptr<actions_map> actions = std::make_shared<actions_map>(
	std::initializer_list<actions_map::value_type>{
		{"exit", std::bind(Exit)},
		{"play", std::bind(Play) },
		{"back_start", std::bind(Back_to_start) },
		{"inc_map", std::bind(Inc_map)},
		{"dec_map", std::bind(Dec_map)}
}
);