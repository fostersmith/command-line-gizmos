#include "flappybird.h"

int main(){
    int in;
    uint8_t over;
    struct timespec start, end;
    long nanoseconds_elapsed;

    Game game = init_flappybird(100, 20, 4, time(NULL));
    ScreenSpec spec = get_screenspec(&game);

    render_init();

    do {
        clock_gettime(CLOCK_MONOTONIC, &start);
        render(&game, &spec);

        while((in = getch()) != ERR){
            if(in == KEY_UP || in == ' ' || in == 'w'){
                game.got_input = 1;
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &end);

        nanoseconds_elapsed = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);

        over = do_timestep(&game, (double)nanoseconds_elapsed);

        //DEBUG
        if(over != 0 )
            game.bird.y = game.h/2.0;
        over = 0;

    } while(over == 0);

    render_end();

    printf("In: %d\n", in);
}