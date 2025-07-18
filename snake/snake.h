#ifndef snake_h
#define snake_h
    #define _XOPEN_SOURCE_EXTENDED 1
    #define _DEFAULT_SOURCE 1
    #define _POSIX_C_SOURCE 199309L

    #include <curses.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <locale.h>
    #include <wchar.h>
    #include <signal.h>
    #include <errno.h>
    #include <time.h>
    #include <langinfo.h>

    #include "../cargparse/cargparse.h"

    #define WIDTH 20
    #define HEIGHT 10

    extern int disable_utf8_warning;
    extern int utf8;

    enum GameState 
    {
        WIN,
        LOSE,
        PLAYING
    };

    enum Input
    {
        NO_INPUT,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    enum TileState
    {
        SNAKE,
        APPLE,
        BLANK
    };

    struct SnakeGame 
    {
        // holds information about snake (used to place apples)
        // should be updated every timestep
        bool game_matrix[WIDTH][HEIGHT];

        // list of snake coordinates
        // head is last entry
        int snake[WIDTH*HEIGHT][2];
        int snake_head_ind;

        // pos of apple (x, y)
        int apple_pos[2];

        // last input
        enum Input last_input;

        // state of game
        enum GameState state;
    };

    //snakelib.c
    struct SnakeGame new_game();
    enum GameState step_game(struct SnakeGame *game, enum Input input);

    //snakerendering.c
    void render_init();
    void render(struct SnakeGame *game);
    int using_utf8();
    void draw_utf8_warning();
    void draw_pause(struct SnakeGame *game);
    void draw_gameover(struct SnakeGame *game);
    void draw_bg();
    void draw_frame();
#endif