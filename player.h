#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEYWORD_LEN 20
#define MAX_NUM_OF_KEYWORD 20

typedef struct{
    char keyword_set[MAX_NUM_OF_KEYWORD][MAX_KEYWORD_LEN];
    int curr_size;
    int cookie_id;
    int is_start;
    int is_paired;
    int is_end;
    int is_game_over;
}player_t;

player_t* create(int cookie_id, int is_paired);
void add_keyword(char* keyword, player_t* player);
int check_keyword_equality(char* keyword, player_t* player);
