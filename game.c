#include "game.h"

game_info_t* create_game_info(){
    game_info_t* game_info = (game_info_t*)malloc(sizeof(game_info_t));
    game_info->player1 = (player_t*)malloc(sizeof(player_t));
    game_info->player2 = (player_t*)malloc(sizeof(player_t));
    game_info->player1 = game_info->player2 = NULL;
    game_info->is_game_end = game_info->is_game_over = 0;
    return game_info;
}

void set_game_over(int* is_game_over, int value){
    *is_game_over = value;
}

void set_game_end(int* is_game_end, int value){
    *is_game_end = value;
}

player_t** get_null_player(game_info_t* game_info){
    // if player 1 is null, return it
    if (game_info->player1 == NULL){
        return &game_info->player1;
    }
    // if player 2 is null, return it
    if (game_info->player2 == NULL){
        return &game_info->player2;
    }
    // else return null
    return NULL;
}


// This function assumes that currently there are two players with different
// cookie id in the game info
player_t* get_another_player(game_info_t* game_info, int cookie_id){
    // player has cookie_id is the current player, we want to return another one
    if (game_info->player1->cookie == cookie_id){
        return game_info->player2;
    }
    return game_info->player1;
}

void register_into_game(game_info_t* game_info, player_t* curr_player){
    player_t** null_player = get_null_player(game_info);
    *null_player = curr_player;
}

void print_game_info(game_info_t* game_info){
    if (game_info->player1 == NULL){
        printf("Player 1 is Null\n");
    }else{
        print_player(game_info->player1);
    }

    if (game_info->player2 == NULL){
        printf("Player 2 is Null\n");
    }else{
        print_player(game_info->player2);
    }

    printf("The game end is %d\n", game_info->is_game_end);
    printf("The game over is %d\n", game_info->is_game_over);
}
