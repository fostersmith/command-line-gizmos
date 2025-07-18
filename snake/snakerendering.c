#include "snake.h"

int utf8 = 0;
int disable_utf8_warning = 0;

int using_utf8(){
    return strcmp(nl_langinfo(CODESET), "UTF-8") == 0;
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
        return 'S';
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
        return
            to == UP ? '^' :
            to == DOWN ? 'v' :
            to == LEFT ? '<' :
            to == RIGHT ? '>' : '?';
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

void print_center_str(int line_offset, char *msg){
    int center_l = LINES/2;
    int center_c = COLS/2;
    int len = strlen(msg);
    mvprintw(center_l+line_offset, center_c-len/2, "%s", msg);
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

void draw_bg()
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

    print_center_str(-1, game->state==LOSE ? "  game over!  " : "  YOU WIN!!!  ");

    char msg[64];
    snprintf(msg, sizeof(msg), "  your score: %d  ", game->snake_head_ind);
    print_center_str(0, msg);
    print_center_str(1, "  press enter key to continue...  ");
}

void draw_pause(struct SnakeGame *game){
    print_center_str(-HEIGHT/2-5, "paused");

    char msg[64];
    snprintf(msg, sizeof(msg), "current score: %d", game->snake_head_ind);
    print_center_str(-HEIGHT/2-4, msg);

    print_center_str(-HEIGHT/2-3, "press p to continue...");
}

void draw_utf8_warning(){
    print_center_str(HEIGHT/2+3, "this game looks better with UTF-8!");
    print_center_str(HEIGHT/2+4, "if your system supports the UTF-8");
    print_center_str(HEIGHT/2+5, "locale, try setting LANG='C.UTF-8'");
    print_center_str(HEIGHT/2+7, "disable this message with --no-utf8-warning");
}

void render(struct SnakeGame *game)
{
    erase();
    draw_bg();
    draw_frame();
    curs_set(0);

    if(!utf8 && !disable_utf8_warning){
        draw_utf8_warning();
    }

    wchar_t wc;
    int x;
    int y;

    // Draw the game
    // - Snake
    for(int i = 0; i < game->snake_head_ind; ++i){
        y = game->snake[i][1];
        x = game->snake[i][0];

        // detect to and from
        enum Input to = NO_INPUT;
        enum Input from = NO_INPUT;

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

void render_init(){
    setlocale(LC_ALL, "");

    initscr();

    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
}