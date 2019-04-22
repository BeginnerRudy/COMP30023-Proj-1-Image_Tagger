#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_MAX_COOKIE_NUM 2
#define MAX_USERNAME_LEN 20
typedef struct{
    int cookie; // The cookie identifier
    char* username;// The username for the player has this cookie
    char** keywords; // the keyword for one player
    int is_start;
    int is_paired;
    int is_game_end;
    int is_game_over;
}player_t;

typedef struct{
    player_t* cookies;
    int curr_size;
    int max_size;
}player_set_t;

player_set_t* create_player_set();
char* find_username(player_set_t* player_set, int cookie_id);
void add_username(player_set_t* player_set,
    int cookie_id, char* username);
void update_memory_of_player_set(player_set_t* player_set,
    int next_cookie_id);
void add_cookie(player_set_t* player_set);
int is_valid_cookie(player_set_t* player_set, int cookie);
void print_all_cookies(player_set_t* player_set);
int does_contain_cookie(char* buff, player_set_t* player_set);
char* get_cookie(char* buff);
