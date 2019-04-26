#include "player.h"

typedef struct{
    player_t* player1; // A pointer to one playing player
    player_t* player2; // A pointer to another playing player
    int is_game_end;// 0 for not fame end, otherwise game end
    int is_game_over;// 0 for not game over, otherwise game end
}game_info_t;

void set_game_over(int* is_game_over, int value);
void set_game_end(int* is_game_end, int value);
player_t* get_null_player(game_info_t* game_info);
player_t* get_another_player(game_info_t* game_info, int cookie_id);
