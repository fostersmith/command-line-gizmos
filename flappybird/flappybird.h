#ifndef flappybird_h
#define flappybird_h
    #define _XOPEN_SOURCE_EXTENDED // needed for ncursesw
    #define _POSIX_C_SOURCE 199309L //needed for nanosleep

    #include <curses.h>

    #include <math.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <time.h>
    #include <locale.h>

    #include "../cargparse/cargparse.h"

    // Constants //
    // Pillar count
    #define PILLAR_C 6
    // Pillar gap height
    #define PILLAR_GAP_H (double)4
    // Pillar speed u/ns
    #define PILLAR_S (double)4.0E-9
    // For collision detection
    #define HITBOX_RADIUS (double)0.5
    // Jump Velocity
    #define JUMP_V (double)1.5E-8
    // Terminal Velocity
    #define TERMINAL_V (double)100//(double)1.5E-8
    // Input Cooldown
    #define INPUT_COOLDOWN_NS (long)1E8
    // Gravitational Constant u/ns^2
    #define GRAVITY (double)-3E-17

    extern int dev_mode;

    // Structs //
    typedef struct {
        double y;
        double vy;
    } Bird;

    typedef struct {
        double x;
        double gap_y;
    } Pillar;

    typedef struct {
        Bird bird;
        Pillar pillars[PILLAR_C];

        uint8_t got_input;
        long input_cooldown;

        const double w;
        const double h;
        const double bird_x;
    } Game;

    typedef struct {
        int window_w;
        int window_h;
        uint8_t utf8;
        double x_scale;
        double y_scale;
    } ScreenSpec;

    // Lib API //
    Game init_flappybird(const double w, const double h, const double bird_x, const long seed);
    uint8_t do_timestep(Game *game, const double delta_time_ns);

    // Render API //
    ScreenSpec get_screenspec(const Game *game);
    void render(const Game *game, const ScreenSpec *s);
    void render_init();
    void render_end();
#endif