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

void print_help(){
    printf("command-line snake, written by Foster Smith\n\n");
    printf("arguments:\n");
    printf("\t--help, -h\t\tshow this screen\n");
    printf("\t--seed, -s\t\tset seed for apple spawning\n");
    printf("\t--no-utf8-warning\tdon't show utf-8 warning message\n\n");
    printf("controls:\n");
    printf("\tarrow Keys:\t\tcontrol snake direction\n");
    printf("\tp / ppace:\t\tpause or unpause game\n");
}

int main(int argc, char *argv[])
{
    int seed = time(NULL);
    for(int i = 1; i < argc; ++i){
        if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0){
            print_help();
            return 0;
        }
        else if(strcmp(argv[i], "--no-utf8-warning") == 0){
            show_utf8_warning = false;
        }
        else if(strcmp(argv[i], "--seed") == 0 || strcmp(argv[i], "-s") == 0){
            if(i+1 < argc){
                seed = atoi(argv[i + 1]);
                i += 1;
            } else {
                printf("Must provide seed\n");
                return 1;
            }
        }

        if(strcmp(argv[i], "--debug") == 0){
            printf("Seed: %d\n", seed);
        }
    }

    srand(seed);

    play_snake();

    return 0;
}

// sudo apt-get install libncurses-dev
// gcc -Wall snake.c -lncursesw -o snake
