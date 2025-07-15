#include<stdlib.h>
#include<string.h>

#define WIDTH 20
#define HEIGHT 10

enum GameState 
{
    WIN,
    LOSE,
    PLAYING
};

enum Input
{
    NO_INPUT,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

enum TileState
{
    SNAKE,
    APPLE,
    BLANK
};

struct SnakeGame 
{
    // holds information about snake (used to place apples)
    // should be updated every timestep
    bool game_matrix[WIDTH][HEIGHT];

    // list of snake coordinates
    // head is last entry
    int snake[WIDTH*HEIGHT][2];
    int snake_head_ind;

    // pos of apple (x, y)
    int apple_pos[2];

    // last input
    enum Input last_input;

    // state of game
    enum GameState state;
};

void update_game_matrix(struct SnakeGame *game){
    // Clear out array
    memset(game->game_matrix, false, sizeof game->game_matrix);

    for(int i = 0; i <= game->snake_head_ind; ++i){
        game->game_matrix[ game->snake[i][0] ][ game->snake[i][1] ] = true;
    }
}

struct SnakeGame new_game()
{
    struct SnakeGame game = {0};

    int mid_x = WIDTH / 2;
    int mid_y = HEIGHT / 2;

    game.snake[0][0] = 1;
    game.snake[1][0] = 2;
    game.snake[2][0] = 3;
    game.snake[0][1] = mid_y;
    game.snake[1][1] = mid_y;
    game.snake[2][1] = mid_y;
    game.snake_head_ind = 2;

    game.apple_pos[0] = mid_x;
    game.apple_pos[1] = mid_y;

    game.last_input = RIGHT;

    game.state = PLAYING;

    update_game_matrix(&game);

    return game;
}

enum GameState step_game(struct SnakeGame *game, enum Input input)
{
    // TODO this is icky
    int next_head[2];
    if(input == NO_INPUT){
        input = game->last_input;
    }
    if(input == LEFT && game->last_input == RIGHT){
        input = game->last_input;
    } else if(input == RIGHT && game->last_input == LEFT){
        input = game->last_input;
    } else if(input == UP && game->last_input == DOWN){
        input = game->last_input;
    } else if(input == DOWN && game->last_input == UP){
        input = game->last_input;
    }
    if(input == RIGHT){
        next_head[0] = game->snake[game->snake_head_ind][0]+1;
        next_head[1] = game->snake[game->snake_head_ind][1];
        game->last_input = RIGHT;
    } else if(input == LEFT){
        next_head[0] = game->snake[game->snake_head_ind][0]-1;
        next_head[1] = game->snake[game->snake_head_ind][1];
        game->last_input = LEFT;
    } else if(input == UP){
        next_head[0] = game->snake[game->snake_head_ind][0];
        next_head[1] = game->snake[game->snake_head_ind][1]+1;
        game->last_input = UP;
    } else if(input == DOWN){
        next_head[0] = game->snake[game->snake_head_ind][0];
        next_head[1] = game->snake[game->snake_head_ind][1]-1;
        game->last_input = DOWN;
    }

    // Lose to wall collision
    if(next_head[0] < 0 || next_head[0] >= WIDTH){
        game->state=LOSE;
        return LOSE;
    }
    if(next_head[1] < 0 || next_head[1] >= HEIGHT){
        game->state=LOSE;
        return LOSE;
    }

    if(next_head[0] == game->apple_pos[0] && next_head[1] == game->apple_pos[1])
    {
        // Apple
        game->snake_head_ind += 1;
        game->snake[game->snake_head_ind][0] = next_head[0];
        game->snake[game->snake_head_ind][1] = next_head[1];
        
        update_game_matrix(game);

        if(game->snake_head_ind+1 == WIDTH*HEIGHT){
            game->state = WIN;
            return WIN;
        }

        do{
            game->apple_pos[0] = rand() % WIDTH;
            game->apple_pos[1] = rand() % HEIGHT;
        }
        while(game->game_matrix[game->apple_pos[0]][game->apple_pos[1]]);
    }
    else
    {
        // No Apple

        // Detect self collision
        if(game->game_matrix[next_head[0]][next_head[1]]){
            if(game->snake[0][0] == next_head[0] && game->snake[0][1] == next_head[1]){
                // False positive - hitting tail which will move with next step
            }
            else {
                game->state = LOSE;
                return LOSE;
            }
        }

        for(int i = 0; i < game->snake_head_ind; ++i)
        {
            game->snake[i][0] = game->snake[i+1][0];
            game->snake[i][1] = game->snake[i+1][1];
        }

        // Set head
        game->snake[game->snake_head_ind][0] = next_head[0];
        game->snake[game->snake_head_ind][1] = next_head[1];
        update_game_matrix(game);
    }

    return PLAYING;
}
