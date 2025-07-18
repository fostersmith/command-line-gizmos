#include "flappybird.h"

// Definitions //

// Transform to ncurse space
static int tx(const double x, const ScreenSpec *s);
static int ty(const double y, const ScreenSpec *s);
static int renderchar(const double x, const double y, wchar_t wc, const ScreenSpec *s);

// Render Helpers
static void render_bird(const Game *game, const ScreenSpec *s);
static void render_pillar(const Game *game, const int pillar_i, const ScreenSpec *s);
static void render_pillars(const Game *game, const ScreenSpec *s);
static void render_frame(const Game *game, const ScreenSpec *s);

// Char Helpers
static wchar_t get_bird_char(const ScreenSpec *s);
static wchar_t get_pillar_char(const ScreenSpec *s);
static wchar_t get_frame_char(const ScreenSpec *s);

// Implementations //

static int tx(const double x, const ScreenSpec *s){
    return round(x*s->x_scale) - s->window_w/2 + COLS/2;
}
static int ty(const double y, const ScreenSpec *s){
    return round(s->window_h/2 - y*s->y_scale + LINES/2);
}
static int mvadd_wchar(const int y, const int x, const wchar_t wc){
    cchar_t cch;
    setcchar(&cch, &wc, A_NORMAL, 0, NULL);
    return mvadd_wch(y, x, &cch);
}
static int renderchar(const double x, const double y, wchar_t wc, const ScreenSpec *s){
    return mvadd_wchar(ty(y, s), tx(x, s), wc);
}

static wchar_t get_bird_char(const ScreenSpec *s){
    return '@';
}
static wchar_t get_pillar_char(const ScreenSpec *s){
    return '|';
}
static wchar_t get_frame_char(const ScreenSpec *s){
    return '+';
}

static void render_bird(const Game *game, const ScreenSpec *s){
    renderchar(game->bird_x, game->bird.y, get_bird_char(s), s);
}
// TODO rendering precision
static void render_pillar(const Game *game, const int pillar_i, const ScreenSpec *s){
    const Pillar *pillar = &game->pillars[pillar_i];

    double bias = fmod(0.1*pillar_i, 1.0);

    // Screenspace coords (inverts y)
    int top_y = ty(pillar->gap_y+PILLAR_GAP_H/2.0, s);
    int bottom_y = ty(pillar->gap_y-PILLAR_GAP_H/2.0, s);
    int screen_bottom = ty(0, s);
    int screen_top = ty(game->h, s);
    int x = tx(pillar->x, s);

    for(int y = bottom_y; y <= screen_bottom; ++y){
        mvadd_wchar(y, x, get_pillar_char(s));
    }

    for(int y = top_y; y >= screen_top; --y){
        mvadd_wchar(y, x, get_pillar_char(s));
    }

    if(dev_mode){
        mvprintw(ty(-2, s), x, "x:%f", pillar->x);
        mvprintw(ty(-3, s), x, "x:%d", x);
    }
}

static void render_pillars(const Game *game, const ScreenSpec *s){
    for(int i = 0; i < PILLAR_C; ++i){
        render_pillar(game, i, s);
    }
}
static void render_frame(const Game *game, const ScreenSpec *s){
    int top_line_y = ty(game->h, s)-1;
    int bot_line_y = ty(0, s)+1;
    int left_line_x = tx(0, s)-1;
    int right_line_x = tx(game->w, s)+1;

    for(int y = top_line_y; y <= bot_line_y; ++y){
        mvadd_wchar(y, left_line_x, get_frame_char(s));
        mvadd_wchar(y, right_line_x, get_frame_char(s));
    }

    for(int x = left_line_x; x <= right_line_x; ++x){
        mvadd_wchar(top_line_y, x, get_frame_char(s));
        mvadd_wchar(bot_line_y, x, get_frame_char(s));
    }
}

void render_init(){
    setlocale(LC_ALL, "");

    initscr();

    setvbuf(stdout,NULL,_IOFBF,8192);

    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
    noecho();
}

void render_end(){
    curs_set(1);
    endwin();
}

ScreenSpec get_screenspec(const Game *game){
    int goal_w = 2*COLS/3;
    int goal_h = 2*LINES/3;

    // screen_w/PILLAR_C should be int to avoid resolution mismatch (i.e. columns appear to move independently)
    // this attempts to scale the window width to play nicely
    int res_w = round(goal_w/PILLAR_C) * PILLAR_C;
    int res_h = (int)((double)res_w / goal_w * goal_h);

    if(
        res_h < 5 || res_h > LINES ||
        res_w < 5 || res_w > COLS
    ){
        res_h = goal_h;
        res_w = goal_w;
    }

    ScreenSpec spec = {
        .window_w = res_w,
        .window_h = res_h,
        .utf8 = 0
    };

    spec.x_scale = (double)spec.window_w / game->w;
    spec.y_scale = (double)spec.window_h / game->h;

    return spec;
}

void render(const Game *game, const ScreenSpec *s){
    erase();
    render_frame(game, s);
    render_pillars(game, s);
    render_bird(game, s);

    if(dev_mode){
        mvprintw(0,0,"Window Width:  %d/%d", s->window_w, COLS);
        mvprintw(1,0,"Window Height: %d/%d", s->window_h, LINES);
    }

    refresh();
}