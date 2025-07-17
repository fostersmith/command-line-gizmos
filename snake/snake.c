#define _XOPEN_SOURCE_EXTENDED

#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <wchar.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <langinfo.h>

#include "snakelib.c"
#include "snakerendering.c"
#include "../cargparse/cargparse.c"

void sleep_ignore_interrupt(int s, int ns){
    struct timespec req = {s, ns}, rem;
    while (nanosleep(&req, &rem) == -1 && errno == EINTR){
        req = rem;
    }
}

enum GameState play_snake(){
    render_init();

    int in_raw;
    enum Input in;
    struct SnakeGame game = new_game();
    enum GameState result = PLAYING;

    while(result == PLAYING){
        render(&game);
        sleep_ignore_interrupt(0, 2E8);

        in_raw = getch();

        // Handle pause
        if(in_raw == 'p' || in_raw == ' '){
            nodelay(stdscr, FALSE);
            draw_pause(&game);
            refresh();
            do{
                in_raw = getch();
            } while (in_raw != 'p' && in_raw != ' ');
            nodelay(stdscr, TRUE);
            continue;
        }

        in =  in_raw == KEY_UP      || in_raw == 'w' ? UP
            : in_raw == KEY_DOWN    || in_raw == 's' ? DOWN
            : in_raw == KEY_LEFT    || in_raw == 'a' ? LEFT
            : in_raw == KEY_RIGHT   || in_raw == 'd' ? RIGHT
            : NO_INPUT;
        
        result = step_game(&game, in);
    }

    draw_gameover(&game);
    refresh();
    usleep(5E5);

    // Clear input buffer
    while(getch() != ERR);

    // Await input
    refresh();
    do{
        in_raw = getch();
        usleep(1E3);
    } while(in_raw != '\n' && in_raw != '\r' && in_raw != ' ');

    endwin();

    return game.state;
}

int main(int argc, char *argv[])
{
    CARGPARSE_MOTD = "terminal snake, written by foster smith";

    setlocale(LC_ALL, "");

    utf8 = using_utf8();
    int seed = time(NULL);

    int debug = 1;
    disable_utf8_warning = 1;

    ArgSpec specs[] = {
        {&disable_utf8_warning, "--no-utf8-warn", "-n", "Disable UTF-8 Warning", ARG_TYPE_BOOL, (void *)1, 0},
        {&utf8, "--utf8", "-u", "Use UTF-8", ARG_TYPE_BOOL, (void *)1, 0},
        {&seed, "--seed", "-s", "Set Seed for Randomization", ARG_TYPE_INT, NULL, 0},
        {&debug, "--debug", "-d", "Debug Mode", ARG_TYPE_BOOL, NULL, 0},
    };

    if(parse_args(argc, argv, 3, specs) != 0) return 1;

    if(debug){
    }

    srand(seed);

    play_snake();

    return 0;
}

// sudo apt-get install libncurses-dev
// gcc -Wall snake.c -lncursesw -o snake
