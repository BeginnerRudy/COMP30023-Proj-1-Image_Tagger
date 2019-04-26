#include "player.h"

typedef struct{
    player_t* player1; // A pointer to one playing player
    player_t* player2; // A pointer to another playing player
    int is_game_end;// 0 for not fame end, otherwise game end
    int is_game_over;// 0 for not game over, otherwise game end
    int num_active_player;// Number of player playing game now.
}game_info_t;

game_info_t* create_game_info();
void set_game_over(int* is_game_over, int value);
void set_game_end(int* is_game_end, int value);
player_t** get_null_player(game_info_t* game_info);
player_t* get_another_player(game_info_t* game_info, int cookie_id);
player_t** get_current_player(game_info_t* game_info, int cookie_id);
void register_into_game(game_info_t* game_info, player_t* curr_player);
void print_game_info(game_info_t* game_info);
void clear_player_game_info(game_info_t* game_info, int cookie_id);
