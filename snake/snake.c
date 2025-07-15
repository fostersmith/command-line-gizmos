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

struct SnakeGame *global_game;
bool utf8;

void sleep_ignore_interrupt(int s, int ns){
    struct timespec req = {s, ns}, rem;
    while (nanosleep(&req, &rem) == -1 && errno == EINTR){
        req = rem;
    }
}

int mvadd_wchar(int y, int x, wchar_t wc)
{
    cchar_t cch;
    setcchar(&cch, &wc, A_NORMAL, 0, NULL);
    return mvadd_wch(y, x, &cch);
}

// center x
int cx(int x){
    return x-WIDTH/2 + COLS/2;
}
// center y
int cy(int y){
    return HEIGHT/2-y + LINES/2;
}

wchar_t get_head_char(enum Input to, bool utf8){
    if(!utf8){
        // ASCII Fallback
        return
            to == UP ? '^' :
            to == DOWN ? 'v' :
            to == LEFT ? '<' :
            to == RIGHT ? '>' : '?';
    } else {
        return
            to == UP ? L'▴' :
            to == DOWN ? L'▾':
            to == LEFT ? L'◂' :
            to == RIGHT ? L'▸' : L'?';
    }
}

wchar_t get_body_char(enum Input to, enum Input from, bool tail, bool utf8){
    if(!utf8){
        // ASCII Fallback
        if(tail){
            from = to;
        }
        wchar_t wc = get_head_char(to, utf8);
        return wc;

        // if(from == UP || from == DOWN){
        //     return (to == UP || to == DOWN) ? '|' : '+';
        // } else if(from == RIGHT || from == LEFT){
        //     return (to == RIGHT || to == LEFT) ? '-' : '+';
        // }
    } else {
        if(tail){
            return to==UP ? L'╵'
                : to==DOWN ? L'╷'
                : to==LEFT ? L'╴'
                : to==RIGHT? L'╶':'?';
        } else if(from == UP){
            return
                to==UP      ? L'!':
                to==DOWN    ? L'│':
                to==LEFT    ? L'┘':
                to==RIGHT   ? L'└'
                :'?';
        } else if (from == DOWN){
            return
                to==UP      ? L'│':
                to==DOWN    ? L'!':
                to==LEFT    ? L'┐':
                to==RIGHT   ? L'┌'
                :'?';
        } else if (from == LEFT){
            return
                to==UP      ? L'┘':
                to==DOWN    ? L'┐':
                to==LEFT    ? L'!':
                to==RIGHT   ? L'─'
                :'?';
        } else if (from == RIGHT){
            return
                to==UP      ? L'└':
                to==DOWN    ? L'┌':
                to==LEFT    ? L'─':
                to==RIGHT   ? L'!'
                :'?';
        }
    }

    return '?';
}


wchar_t get_apple_char(bool utf8){
    return utf8 ? L'•' : 'O';
}

wchar_t get_bg_char(bool utf8){
    return utf8 ? L'·' : '\'';
}

wchar_t get_border_char(enum Input side, bool utf8){
    if(!utf8){
        return (side == UP || side == DOWN) ? '=' : '|';
    } else {
        return (side == UP || side == DOWN) ? L'─' : L'│';
    }
}

wchar_t get_corner_char(enum Input lr, enum Input ud, bool utf8){
    if(!utf8){
        return '+';
    } else {
        if(lr == LEFT){
            return ud == UP ? L'┌' : L'└';
        } else {
            return ud == UP ? L'┐' : L'┘';
        }
    }
}

void draw_frame()
{
    // Bottom Left
    mvadd_wchar(cy(-1), cx(-1), get_corner_char(LEFT, DOWN, utf8));
    // Bottom Right
    mvadd_wchar(cy(-1), cx(WIDTH), get_corner_char(RIGHT, DOWN, utf8));
    // Top Left
    mvadd_wchar(cy(HEIGHT), cx(-1), get_corner_char(LEFT, UP, utf8));
    // Top Right
    mvadd_wchar(cy(HEIGHT), cx(WIDTH), get_corner_char(RIGHT, UP, utf8));

    wchar_t wc = get_border_char(UP, utf8);
    for(int x = 0; x < WIDTH; ++x){
        mvadd_wchar(cy(-1), cx(x), wc);
        mvadd_wchar(cy(HEIGHT), cx(x), wc);
    }
    wc = get_border_char(LEFT, utf8);
    for(int y = 0; y < HEIGHT; ++y){
        mvadd_wchar(cy(y), cx(-1), wc);
        mvadd_wchar(cy(y), cx(WIDTH), wc);
    }
}

void clear_content()
{
    wchar_t dot_char = get_bg_char(utf8);
    wchar_t* dots = malloc((WIDTH + 1)*sizeof(wchar_t));
    wmemset(dots, dot_char, WIDTH);

    attron(A_DIM);
    for(int l = 0; l < HEIGHT; ++l){
        mvaddwstr(cy(l), cx(0), dots);
    }
    attroff(A_DIM);

    free(dots);
}

void draw_gameover(struct SnakeGame *game){
    char msg[64];
    int len;

    int center_l = LINES/2;
    int center_c = COLS/2;

    snprintf(msg, sizeof(msg), game->state==LOSE ? "GAME OVER!" : "YOU WIN!!!");
    len = strlen(msg);
    mvprintw(center_l-1, center_c-len/2, "%s", msg);

    snprintf(msg, sizeof(msg), "YOUR SCORE: %d", game->snake_head_ind);
    len = strlen(msg);
    mvprintw(center_l, center_c-len/2, "%s", msg);

    snprintf(msg, sizeof(msg), "PRESS ANY KEY TO CONTINUE...");
    len = strlen(msg);
    mvprintw(center_l+1, center_c-len/2, "%s", msg);
}

void draw_pause(struct SnakeGame *game){
    char msg[64];
    int len;

    int center_l = LINES/2;
    int center_c = COLS/2;

    snprintf(msg, sizeof(msg), "PAUSED");
    len = strlen(msg);
    mvprintw(center_l-1, center_c-len/2, "%s", msg);

    snprintf(msg, sizeof(msg), "CURRENT SCORE: %d", game->snake_head_ind);
    len = strlen(msg);
    mvprintw(center_l, center_c-len/2, "%s", msg);

    snprintf(msg, sizeof(msg), "PRESS P TO CONTINUE...");
    len = strlen(msg);
    mvprintw(center_l+1, center_c-len/2, "%s", msg);
}

void render(struct SnakeGame *game)
{
    clear_content();
    curs_set(0);

    wchar_t wc;
    int x;
    int y;

    // Draw the game
    // - Snake
    for(int i = 0; i < game->snake_head_ind; ++i){
        y = game->snake[i][1];
        x = game->snake[i][0];

        // detect to and from
        enum Input to;
        enum Input from;

        if(i != 0){
            if(game->snake[i-1][0] == game->snake[i][0]){
                if(game->snake[i-1][1] > game->snake[i][1]){
                    from = UP;
                } else {
                    from = DOWN;
                }
            } else {
                if(game->snake[i-1][0] > game->snake[i][0]){
                    from = RIGHT;
                } else {
                    from = LEFT;
                }
            }
        }

        if(game->snake[i+1][0] == game->snake[i][0]){
            if(game->snake[i+1][1] > game->snake[i][1]){
                to = UP;
            } else {
                to = DOWN;
            }
        } else {
            if(game->snake[i+1][0] > game->snake[i][0]){
                to = RIGHT;
            } else {
                to = LEFT;
            }
        }

        wc = get_body_char(to, from, i==0, utf8);

        mvadd_wchar(cy(y), cx(x), wc);
    }
    // -- Head
    y = game->snake[game->snake_head_ind][1];
    x = game->snake[game->snake_head_ind][0];
    wc = get_head_char(game->last_input, utf8);
    mvadd_wchar(cy(y), cx(x), wc);
    

    // - Apple
    wc = get_apple_char(utf8);
    mvadd_wchar(cy(game->apple_pos[1]), cx(game->apple_pos[0]), wc);

    move(0,0);

    refresh();
    curs_set(1);
}

void do_resize(int sig){
    endwin();
    clear();
    draw_frame();
    render(global_game);
    refresh();
}

int main(void)
{
    setlocale(LC_ALL, "");

    initscr();

    setvbuf(stdout,NULL,_IOFBF,8192);

    utf8 = strcmp(nl_langinfo(CODESET), "UTF-8") == 0; // detect utf8 availability

    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    noecho();

    // signal(SIGWINCH, do_resize);

    int in_raw;
    enum Input in;

    struct SnakeGame game = new_game();
    global_game = &game;

    enum GameState result = PLAYING;

    // Draw the border
    draw_frame();
    while(result == PLAYING){
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
        clear();
        draw_frame();
        render(&game);
        sleep_ignore_interrupt(0, 3E8);
    }

    draw_gameover(&game);
    refresh();
    usleep(500000);

    getch();

    nodelay(stdscr, FALSE);
    refresh();
    while(getch() == ERR){
        usleep(1000);
    }

    endwin();
}

// gcc -Wall snake.c -lncursesw -o snake