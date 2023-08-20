#include <stdlib.h>

#define WORLD(game, x, y) (game)->level.objects[(y)*(game)->level.width + (x)]

#define HEIGHT 10
#define WIDTH (2*HEIGHT)

char *BOMB_LEVEL[] = {
	"          ",
	"##########",
	"    @     ",
	"##########",
	"          ",
};

char *LEVEL[] = {
	"    #     ",
	"    #   ##",
	"    #   ##",
	"    ##  ##",
	"          ",
};

Game_Object first_level_objects[WIDTH*HEIGHT];

Game_Level first_level = {
	.objects = first_level_objects,
	.width = WIDTH,
	.height = HEIGHT,
	.start_x = 4,
	.start_y = 4,
};

inline void start_level(Game *game, Game_Level level)
{
	game->level = level;
	game->x = level.start_x;
	game->y = level.start_y;
}

size_t count_chars_until(char *s, char c)
{
	size_t n = 0;
	while (s[n] != '\0') {
		n += 1;
		if (s[n-1] == c) break;
	}
	return n;
}

Game_Level parse_level(char *input[], size_t width, size_t height)
{
	Game_Level level = {0};
		
	level.width = width;
	level.height = height;	
	level.objects = calloc(width*height, sizeof(Game_Object));
	
	for (size_t y = 0; y < height; ++y) {
		for (size_t x = 0; x < width; ++x) {
			
			size_t i = y*width + x;
			switch (input[y][x]) {
			case '#':
				level.objects[i].type = BOUNDARY;
				break;
			case ' ':
				break;
			default:
				assert(0);
			}
		}
	}
	
	return level;
}