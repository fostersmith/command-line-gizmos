#include "flappybird.h"

int dev_mode;

static int game_loop(){
    int in;
    uint8_t over;
    struct timespec start, end;
    long nanoseconds_elapsed;

    Game game = init_flappybird(100, 20, 4, time(NULL));
    ScreenSpec spec = get_screenspec(&game);

    do {
        clock_gettime(CLOCK_MONOTONIC, &start);
        render(&game, &spec);

        while((in = getch()) != ERR){
            if(in == KEY_UP || in == ' ' || in == 'w'){
                game.got_input = 1;
            }

            if(dev_mode){
                game.got_input = 0;
                if(in == KEY_UP || in == ' ' || in == 'w'){
                    game.bird.y += (game.h)/spec.window_h;
                } else if(in == KEY_DOWN || in == 's'){
                    game.bird.y -= (game.h)/spec.window_h;
                }
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &end);

        nanoseconds_elapsed = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);

        over = do_timestep(&game, (double)nanoseconds_elapsed);

        // DEBUGS HERE
        if(dev_mode)
        {
            if(over == 1){
                mvprintw(2, 0, "DEAD");
                refresh();
            }
            over = 0;
        }

    } while(over == 0);

    return 0;
}

int main(int argc, char *argv[]){
    ArgSpec specs[] = {
        {&dev_mode, "--dev-mode", "-d", "Developer Mode", ARG_TYPE_BOOL, (void *)0, 0},
    };
    if(parse_args(argc, argv, 1, specs) != 0) return 1;

    render_init();
    game_loop();
    render_end();

    return 0;
}