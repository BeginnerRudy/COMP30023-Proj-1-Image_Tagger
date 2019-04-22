#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_MAX_PLAYER_NUM 2
#define MAX_USERNAME_LEN 20
#define MAX_KEYWORD_LENGTH 20
#define MAX_NUM_OF_KEYWORD 20
typedef struct{
    int cookie; // The cookie identifier
    char* username;// The username for the player has this cookie
    char** keywords; // the keyword for one player
    int is_start;
    int is_paired;
    int is_game_end;
    int is_game_over;
    int curr_keyword_count;
}player_t;

typedef struct{
    player_t* players;
    int curr_size;
    int max_size;
}player_set_t;
#define KEYWORD_STOP_CHAR '&'


player_set_t* create_player_set();
void initialise_players(player_t* players, int player_start_index, int player_end_index);
char* find_username(player_set_t* player_set, int cookie_id);
void add_username(player_set_t* player_set,
    int cookie_id, char* username);
void update_memory_of_player_set(player_set_t* player_set,
    int next_cookie_id);
void add_cookie(player_set_t* player_set);
void add_keyword(player_set_t* player_set, int cookie_id, char* keyword);
int is_valid_cookie(player_set_t* player_set, int cookie);
void print_all_cookies(player_set_t* player_set);
int does_contain_cookie(char* buff, player_set_t* player_set);
char* get_cookie(char* buff);char* parse_and_format_keyword(char* buff);
