#include "player.h"

player_t* create(int cookie_id, int is_paired){
    player_t* player = (player_t*)malloc(sizeof(player_t));
    player->curr_size = 0;
    player->cookie_id = cookie_id;
    player->is_start = 0;
    player->is_paired = is_paired;
    player->is_end = 0;
    player->is_game_over = 0;
    return player;
}

void add_keyword(char* keyword, player_t* player) {
    int index = player->curr_size++;
    strncpy(player->keyword_set[index], keyword, strlen(keyword));
}

int check_keyword_equality(char* keyword, player_t* player){
    for (int i = 0; i < player->curr_size; i++){
        if (strncmp(player->keyword_set[i], keyword, strlen(keyword))){
            return 1;
        }
    }
    return 0;
}
