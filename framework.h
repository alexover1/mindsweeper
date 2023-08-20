#pragma once

typedef enum Game_Object_Type {
	EMPTY = 0,
	BOUNDARY = 1,
	RUNNING_SOLDIER = 2,
	COUNTDOWN_BOMB = 3,
} Game_Object_Type;

typedef struct Game_Object {
	Game_Object_Type type;
	int user_data; // Usage depends on the kind of game object. TODO: Document all of the usages
} Game_Object;

typedef struct Game_Level {
	Game_Object *objects;
	size_t width, height;
	int start_x, start_y;
} Game_Level;

typedef struct {
	bool quit;
	int x, y; // Player position
	Game_Level level;
} Game;

bool check_boundary(Game *game, int x, int y);

void handle_player_collision(Game *game);
bool handle_input(Game *game, int command);
void update(Game *game);

char render_game_object(Game_Object object);
void render(const Game *game);

// Level interace
void start_level(Game *game, Game_Level level);
Game_Level parse_level(char *input[], size_t width, size_t height);
