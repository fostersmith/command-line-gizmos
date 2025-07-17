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
static wchar_t get_bg_char(const ScreenSpec *s);
static wchar_t get_frame_char(const ScreenSpec *s);

// Implementations //

static int tx(const double x, const ScreenSpec *s){
    return x*s->x_scale - s->window_w/2 + COLS/2;
}
static int ty(const double y, const ScreenSpec *s){
    return s->window_h/2 - y*s->y_scale + LINES/2;
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
static wchar_t get_bg_char(const ScreenSpec *s){
    return ' ';
}
static wchar_t get_frame_char(const ScreenSpec *s){
    return '+';
}

static void render_bird(const Game *game, const ScreenSpec *s){
    renderchar(game->bird_x, game->bird.y, get_bird_char(s), s);
}
static void render_pillar(const Game *game, const int pillar_i, const ScreenSpec *s){
    const Pillar *pillar = &game->pillars[pillar_i];
    renderchar(pillar->x, pillar->gap_y+PILLAR_GAP_H/2.0, get_pillar_char(s), s);
    renderchar(pillar->x, pillar->gap_y-PILLAR_GAP_H/2.0, get_pillar_char(s), s);
}
static void render_pillars(const Game *game, const ScreenSpec *s){
    for(int i = 0; i < PILLAR_C; ++i){
        render_pillar(game, i, s);
    }
}
static void render_frame(const Game *game, const ScreenSpec *s){
    int top_line_y = LINES/2 + s->window_h/2 + 1;
    int bot_line_y = LINES/2 - s->window_h/2 - 1;
    // int left_line_x = COLS/2 - s->window_w/2 - 1;
    // int right_line_x = COLS/2 + s->window_w/2 + 1;

    int left_line_x = 10;
    int right_line_x = 20;

    for(int y = bot_line_y; y <= top_line_y; ++y){
        mvadd_wchar(y, left_line_x, get_frame_char(s));
        mvadd_wchar(y, right_line_x, get_frame_char(s));
    }

    for(int x = left_line_x; x <= right_line_x; ++x){
        mvadd_wchar(x, top_line_y, get_frame_char(s));
        mvadd_wchar(x, bot_line_y, get_frame_char(s));
    }
}

void render_init(){
    setlocale(LC_ALL, "");

    initscr();

    setvbuf(stdout,NULL,_IOFBF,8192);

    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    noecho();
}

void render_end(){
    endwin();
}

ScreenSpec get_screenspec(const Game *game){
    ScreenSpec spec = {
        .window_w = 100,
        .window_h = 20,
        .utf8 = 0
    };

    spec.x_scale = (double)spec.window_w / game->w;
    spec.y_scale = (double)spec.window_h / game->h;

    return spec;
}

void render(const Game *game, const ScreenSpec *s){
    erase();
    render_frame(game, s);
    // render_pillars(game, s);
    // render_bird(game, s);
    refresh();
}