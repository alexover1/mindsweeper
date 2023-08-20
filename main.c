/*******************************************************************************************
*
*   mindsweeper - minesweeper/roguelike
*
*   Terminal puzzle game developed by Alexander Overstreet
*
*   This game has been created using only libc and POSIX/Windows API
*   Licensed under an unmodified MIT license (View LICENSE for details)
*
*   Copyright (c) 2023 Alexander Overstreet (@traceover)
*
********************************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define MAX_ENTITIES 16   // Maximum number of alive entities

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define DEF   "\x1B[0m"

#define SWAP(T, a, b) do { T t = a; a = b; b = t; } while (0)
#define SIGN(T, x)    ((T)((x) > 0) - (T)((x) < 0))
#define ABS(T, x)     (SIGN(T, x)*(x))

#define MAX(T, a, b) ((a > b) ? a : b)
#define MIN(T, a, b) ((a > b) ? b : a)

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Vec2 {
	int x, y;
} Vec2;

bool vec2_eq(Vec2 a, Vec2 b);
void vec2_add(Vec2 *d, Vec2 a, Vec2 b);
void vec2_sub(Vec2 *d, Vec2 a, Vec2 b);

typedef enum Tag {
	UNINITIALIZED  = 0,
	MOVING_SOLDIER = 1,
	TIMER_BOMB     = 2,
} Tag;

typedef struct Moving_Soldier {
	Vec2 start;  // Original position
	Vec2 delta;  // Distance from start position
	int dir;
} Moving_Soldier;

typedef struct Timer_Bomb {
	int timer;
} Timer_Bomb;

typedef union Subclass {
	Moving_Soldier moving_soldier;
	Timer_Bomb     timer_bomb;
} Subclass;

typedef struct Bit_Array {
	uint64_t chunks[4]; // Stores a maximum of 256 bits
} Bit_Array;

// Implementation of Bit_Array
void bit_array_enable(Bit_Array *bits, size_t i);
bool bit_array_contains(const Bit_Array *bits, size_t i);

typedef struct Level {
	Bit_Array collision;
	
	// Structure of arrays
	Vec2      e_pos[MAX_ENTITIES];
	Tag       e_tag[MAX_ENTITIES];
	Subclass  e_sub[MAX_ENTITIES];
	
	int       num_entities;
} Level;

int  create_entity(void);
void delete_entity(int);

bool check_boundary(int x, int y);
bool move_player_checked(int dx, int dy);

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static int screen_width = 10;
static int screen_height = 5;        // Looks good when using screen_width/2
static bool game_over = false;
static bool quit = false;
static int key_this_frame;           // The input key we pressed this frame

static Level level = {0};
static Vec2 player = {0};

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void init_game(void);         // Initialize game
static void update_game(void);       // Update game (one frame)
static void draw_game(void);         // Draw game (one frame)
static void unload_game(void);       // Unload game
static void update_draw_frame(void); // Update and Draw (one frame)

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	// Setup the terminal environment
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
   	tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_attr);
#endif

	init_game();
	
#if 0
	const int i = create_entity();
	level.e_pos[i] = (Vec2){5, 0};
	level.e_tag[i] = TIMER_BOMB;
	level.e_sub[i].timer_bomb = (Timer_Bomb){
		.timer = 5,
	};
	
	for (int x = 0; x < screen_width; ++x) {
		bit_array_enable(&level.collision, 1*screen_width + x);
	}
#else
	player.y = 1;

	const int i = create_entity();
	level.e_pos[i] = (Vec2){4, 1};
	level.e_tag[i] = MOVING_SOLDIER;
	level.e_sub[i].moving_soldier = (Moving_Soldier){
		.start = level.e_pos[i],
		.delta = {0, 1},
		.dir = 1,
	};
	
	{
		const int x = 4;
		bit_array_enable(&level.collision, 0*screen_width + x);
		bit_array_enable(&level.collision, 4*screen_width + x);
	}
#endif

	while (!quit) {
		update_draw_frame();
	}
	
	// Restore the original console mode
#ifdef _WIN32
	if (!SetConsoleMode(hInput, mode)) {
		perror("SetConsoleMode");
		return 1;
	}
#else
	tcsetattr(STDIN_FILENO, TCSANOW, &saved_attr);
#endif

	return 0;
}

//--------------------------------------------------------------------------------------
// Game Module Functions Definition
//--------------------------------------------------------------------------------------

// Initialize game variables
void init_game(void)
{
	// Load and Parse levels
}

void update_game(void)
{
	if (game_over) return;

	bool moved = false;

	// Handle Input from the User
	switch (key_this_frame) {
	case 'w':
		moved = move_player_checked(0, -1);
		break;
	case 's':
		moved = move_player_checked(0, 1);
		break;
	case 'a':
		moved = move_player_checked(-1, 0);
		break;
	case 'd':
		moved = move_player_checked(1, 0);
		break;
	}
	
	if (!moved) return;
	
	// Check for Collision after Moving
	for (int i = 0; i < level.num_entities; ++i) {
		Vec2 pos = level.e_pos[i];
		switch (level.e_tag[i]) {
		case UNINITIALIZED: break;
		case MOVING_SOLDIER: {
			Moving_Soldier *soldier = &level.e_sub[i].moving_soldier;
			
			Vec2 v = {
				soldier->dir * SIGN(int, soldier->delta.x),
				soldier->dir * SIGN(int, soldier->delta.y)
			};
			
			Vec2 d;
			vec2_sub(&d, pos, soldier->start);
			
			if (vec2_eq(d, soldier->delta)) {
				soldier->dir *= -1;
			}
			
			vec2_add(&level.e_pos[i], pos, v);
			
			if (vec2_eq(player, level.e_pos[i])) {
				game_over = true;
				continue;
			}
		} break;
			
		case TIMER_BOMB: {
			Timer_Bomb *bomb = &level.e_sub[i].timer_bomb;
		
			if (bomb->timer-- == 1) {
				if (vec2_eq(player, pos)) {
					game_over = true;
					continue;
				}
				delete_entity(i);
				i -= 1;
				continue;
			}
		} break;
		}
	}
}

void draw_game(void)
{
	if (game_over) {
		printf(RED);
	}
	
	char screen[screen_height][screen_width];
	
	// Draw the Grid
	for (int y = 0; y < screen_height; ++y) {
		for (int x = 0; x < screen_width; ++x) {
			if (bit_array_contains(&level.collision, y*screen_width + x)) {
				screen[y][x] = '+';
			} else {
				screen[y][x] = ' ';
			}
		}
	}
	
	// Draw the Player
	screen[player.y][player.x] = 'o';

	// Draw the Objects
	for (size_t i = 0; i < level.num_entities; ++i) {
		const int x = level.e_pos[i].x;
		const int y = level.e_pos[i].y;
		const Subclass data = level.e_sub[i];
		
		switch (level.e_tag[i]) {
		case UNINITIALIZED: break;
		
		case MOVING_SOLDIER: {
			screen[y][x] = '%';
		} break;
		
		case TIMER_BOMB: {
			int timer = data.timer_bomb.timer;			
			assert(0 <= timer && timer <= 9);
			screen[y][x] = '0' + timer;
		} break;
		}
	}
	
	// Print the Screen to the Console
	for (int y = 0; y < screen_height; ++y) {
		printf("%.*s\n", screen_width, screen[y]);
	}
	
	printf(DEF); // Reset coloring
}

void unload_game(void)
{

}

void update_draw_frame(void)
{
	draw_game();

	key_this_frame = fgetc(stdin);
	if (key_this_frame == 'q') {
		quit = true;
		return;
	}
		
	update_game();
		
	// Move the Terminal Cursor
	printf("\e[%dA", screen_height);
	printf("\e[%dD", screen_width);
}

//--------------------------------------------------------------------------------------
// Math Functions Definition
//--------------------------------------------------------------------------------------
inline bool vec2_eq(Vec2 a, Vec2 b)
{
	return a.x == b.x && a.y == b.y;
}

inline void vec2_add(Vec2 *d, Vec2 a, Vec2 b)
{
	d->x = a.x + b.x;
	d->y = a.y + b.y;
}

inline void vec2_sub(Vec2 *d, Vec2 a, Vec2 b)
{
	d->x = a.x - b.x;
	d->y = a.y - b.y;
}

inline void bit_array_enable(Bit_Array *bits, size_t i)
{
	bits->chunks[i/64] |= 1ULL<<(i%64);
}

inline bool bit_array_contains(const Bit_Array *bits, size_t i)
{
	return (bits->chunks[i/64]>>(i%64))&1;
}

//--------------------------------------------------------------------------------------
// Internal Functions Definition
//--------------------------------------------------------------------------------------
inline int create_entity(void)
{
	assert(level.num_entities < MAX_ENTITIES);
	return level.num_entities++;
}

inline void delete_entity(int id)
{
	if (level.num_entities <= 1) {
		level.num_entities = 0;
		return;
	}

	const int end = --level.num_entities;

	// Swap with the Last Element
	level.e_pos[id] = level.e_pos[end];
	level.e_tag[id] = level.e_tag[end];
	level.e_sub[id] = level.e_sub[end];
}

inline bool check_boundary(int x, int y)
{
	if (x < 0 || x >= screen_width)  return false;
	if (y < 0 || y >= screen_height) return false;
	return !bit_array_contains(&level.collision, y*screen_width + x);
}

inline bool move_player_checked(int dx, int dy)
{
	// Branchless to go fast
	const int inside = check_boundary(player.x + dx, player.y + dy);
	const Vec2 d = {dx*inside, dy*inside};
	vec2_add(&player, player, d);
	return inside;
}