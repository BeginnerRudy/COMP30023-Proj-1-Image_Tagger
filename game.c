#include "game.h"

void set_game_over(int* is_game_over, int value){
    *is_game_over = value;
}

void set_game_end(int* is_game_end, int value){
    *is_game_end = value;
}

player_t* get_null_player(game_info_t* game_info){
    // if player 1 is null, return it
    if (game_info->player1 == NULL){
        return game_info->player1;
    }
    // if player 2 is null, return it
    if (game_info->player2 == NULL){
        return game_info->player2;
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
