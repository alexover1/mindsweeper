// Note: This is NOT for checking if we are walking into a wall,
// this is what HAPPENS when we move onto another object
// (in most cases it kills you)
void handle_player_collision(Game *game)
{
	Game_Object *collider = &WORLD(game, game->x, game->y);
	switch (collider->type) {
	case EMPTY:
		break;
	case BOUNDARY:
		// If we somehow find ourselves inside of a boundary, just restart the level
		break;
	case RUNNING_SOLDIER:
		start_level(game, game->level);
		break;
	case COUNTDOWN_BOMB:
		// Bombs only explode when their timer runs out
		// (user_data is bomb.timer)
		if (collider->user_data <= 0) {
			start_level(game, game->level);
		}
		break;
	}
}

inline bool check_boundary(Game *game, int x, int y)
{
	if (x < 0 || x >= game->level.width)  return false;
	if (y < 0 || y >= game->level.height) return false;
	return WORLD(game, x, y).type != BOUNDARY;
}

bool handle_input(Game *game, int command)
{
	switch (command) {
	case 'q':
		game->quit = true;
		return true;
	case 'w':
		if (check_boundary(game, game->x, game->y - 1)) {
			game->y -= 1;
		}
		break;
	case 'a':
		if (check_boundary(game, game->x - 1, game->y)) {
			game->x -= 1;
		}
		break;
	case 's':
		if (check_boundary(game, game->x, game->y + 1)) {
			game->y += 1;
		}
		break;
	case 'd':
		if (check_boundary(game, game->x + 1, game->y)) {
			game->x += 1;
		}
		break;
	}
	return false;
}

void update(Game *game)
{
	handle_player_collision(game);
}
