#include "flappybird.h"

// Templates //
static double dist(double x1, double y1, double x2, double y2);
static Pillar get_pillar(const double x, const int h);
static uint8_t detect_collision(const Game *game);
static int apply_physics(Game *game, const double delta_time_ns);
static void update_pillars(Game *game, const double delta_time_ns);

// Utils //
static Pillar get_pillar(const double x, const int game_h){
    return (Pillar){x, rand() % game_h};
}
static double dist(double x1, double y1, double x2, double y2){
    double dx = x2-x1;
    double dy = y2-y1;
    return sqrt(dx*dx+dy*dy);
}
static uint8_t detect_collision(const Game *game){
    for(int i = 0; i < PILLAR_C; ++i){
        Pillar pillar = game->pillars[i];
        if(fabs(pillar.x - game->bird_x) <= HITBOX_RADIUS){ // check horizontal distance before checking euclidean
            double top_dist = dist(pillar.x, pillar.gap_y+PILLAR_GAP_H/2.0, game->bird_x, game->bird.y);
            double bottom_dist = dist(pillar.x, pillar.gap_y-PILLAR_GAP_H/2.0, game->bird_x, game->bird.y);
            if(top_dist <= HITBOX_RADIUS || bottom_dist <= HITBOX_RADIUS){
                return 1;
            }
        }
    }
    return 0;
}
static int apply_physics(Game *game, const double delta_time_ns){
    Bird *bird = &game->bird;

    double v0 = bird->vy;
    double y0 = bird->y;

    double delta_v = GRAVITY*delta_time_ns;
    double delta_y = v0*delta_time_ns + 0.5*GRAVITY*delta_time_ns*delta_time_ns;

    bird->vy = v0 + delta_v;
    if(bird->vy < -TERMINAL_V){
        bird->vy = -TERMINAL_V;
    }
    bird->y = y0 + delta_y;

    if( bird->y < 0 || bird->y > game->h){
        return 1;
    } else {
        return 0;
    }
}
static void update_pillars(Game *game, const double delta_time_ns){
    Pillar *pillars = game->pillars;

    double pillar_dist = PILLAR_S*delta_time_ns;

    for(int i = 0; i < PILLAR_C; ++i){
        Pillar *pillar = &pillars[i];
        pillar->x -= pillar_dist;
        if(pillar->x < 0) {
            pillars[i] = get_pillar(game->w+pillar->x, game->h);
        }
    }
}

// API //
Game init_flappybird(const double w, const double h, const double bird_x, const long seed){
    srand(seed);

    Game game = {
        .bird = { .y = h / 2.0, .vy = JUMP_V },
        .got_input = 0,
        .w = w,
        .h = h,
        .bird_x = bird_x
    };

    for (int i = 0; i < PILLAR_C; ++i){
        double x = i * (w / (double)PILLAR_C);
        game.pillars[i] = get_pillar(x, h);
    }

    return game;
}

// Returns 0 for continue game, 1 for end
uint8_t do_timestep(Game *game, const double delta_time_ns){
    // This doesn't handle large double_time_ns, bird will phase through pillars with large enough delta_time_ns
    game->input_cooldown -= delta_time_ns;
    if(game->input_cooldown <= 0) game->input_cooldown = 0;

    if(game->got_input){
        if(game->input_cooldown > 0){
            game->got_input = 0;
        } else {
            game->bird.vy = JUMP_V;
            game->input_cooldown = INPUT_COOLDOWN_NS;
        }
    }

    if(apply_physics(game, delta_time_ns) != 0) return 1;

    update_pillars(game, delta_time_ns);
    
    return detect_collision(game);
}
