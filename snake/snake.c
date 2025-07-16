#define _XOPEN_SOURCE_EXTENDED

#include<curses.h>
#include<stdlib.h>
#include<unistd.h>
#include<locale.h>
#include<wchar.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <langinfo.h>

#include "snakelib.c"
#include "snakerendering.c"

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
        sleep_ignore_interrupt(0, 3E8);

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

        in = in_raw == KEY_UP ? UP
            : in_raw == KEY_DOWN ? DOWN
            : in_raw == KEY_LEFT ? LEFT
            : in_raw == KEY_RIGHT ? RIGHT
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

void print_help(){
    printf("Command-line snake! Written by Foster Smith\n\n");
    printf("Arguments:\n");
    printf("\t--help, -h\t\tShow this screen\n");
    printf("\t--no-utf8-warning\tDon't print UTF-8 warning message\n\n");
    printf("Controls:\n");
    printf("\tArrow Keys:\t\tControl Snake Direction\n");
    printf("\tP / Space:\t\tPause or Unpause Game\n");
}

int main(int argc, char *argv[])
{
    for(int i = 1; i < argc; ++i){
        if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0){
            print_help();
            return 0;
        }
        if(strcmp(argv[i], "--no-utf8-warning") == 0){
            show_utf8_warning = false;
        }
    }

    play_snake();

    printf("End.\n");
    sleep(5);
    return 0;
}

// gcc -Wall snake.c -lncursesw -o snake