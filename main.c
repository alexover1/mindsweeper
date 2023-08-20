#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h> // TODO: Windows version of isatty()
#endif

#include "framework.h"
#include "game.c"
#include "update.c"
#include "render.c"

int main(void)
{
#ifdef _WIN32
	HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE);
    if (input_handle == INVALID_HANDLE_VALUE) {
		perror("GetStdHandle");
		return 1;
	}

	DWORD mode;
	if (!GetConsoleMode(input_handle, &mode)) {
		perror("GetConsoleMode");
		return 1;
	}
	
	DWORD new_mode = mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    if (!SetConsoleMode(input_handle, new_mode)) {
        perror("SetConsoleMode");
        return 1;
    }
    
    char read_buffer[1];
#else
	if (!isatty(STDIN_FILENO)) {
		fprintf(stderr, "Error: This must be run in a terminal!");
		return 1;
	}
	
	struct termios new_attr, saved_attr;
	
	tcgetattr(STDIN_FILENO, &new_attr);
   	tcgetattr(STDIN_FILENO, &saved_attr);
   	
   	new_attr.c_lflag    &= (~ (ICANON | ECHO));
   	new_attr.c_cc[VMIN]  = 1;
   	new_attr.c_cc[VTIME] = 0;
   	assert(tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_attr) == 0);
#endif

	Game game = {0};
	start_level(&game, parse_level(LEVEL, strlen(LEVEL[0]), sizeof(LEVEL)/sizeof(LEVEL[0])));

	render(&game);
	while (!game.quit) {
		int c = fgetc(stdin);
		handle_input(&game, c);

		printf("\e[%dA", game.level.height);
		printf("\e[%dD", game.level.width*3);
		render(&game);
	}
	
	
#ifdef _WIN32
	// Restore the original console mode
	if (!SetConsoleMode(hInput, mode)) {
		perror("SetConsoleMode");
		return 1;
	}
#else
	tcsetattr(STDIN_FILENO, TCSANOW, &saved_attr);
#endif

	return 0;
}