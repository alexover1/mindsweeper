char render_game_object(Game_Object object)
{
	// Renders ' ' on Empty or invalid data
	switch (object.type) {
	case EMPTY:           return ' ';
	case BOUNDARY:        return '#';
	case RUNNING_SOLDIER: return '!';
	case COUNTDOWN_BOMB:
		assert(object.user_data <= 9);
		return '0' + object.user_data;
	}
	return ' ';
}

void render(const Game *game)
{
	for (int y = 0; y < game->level.height; ++y) {
		for (int x = 0; x < game->level.width; ++x) {	
			if (x == game->x && y == game->y) {
				// We are the player.
				putc('^', stdout);
				continue;
			}
		
			char c = render_game_object(WORLD(game, x, y));
			putc(c, stdout);
		}
		putc('\n', stdout);
	}
}
