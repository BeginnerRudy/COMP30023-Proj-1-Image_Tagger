#include "player.h"
player_set_t* create_player_set(){
    //Cookie, username initilization
    player_set_t* player_set = (player_set_t*)malloc(sizeof(player_set_t));
    player_set->curr_size = 0;
    player_set->max_size = INITIAL_MAX_PLAYER_NUM;
    player_set->players = (player_t*)malloc(INITIAL_MAX_PLAYER_NUM*sizeof(player_t));
    initialise_players(player_set->players, 0, INITIAL_MAX_PLAYER_NUM);
    return player_set;
}

void initialise_players(player_t* players, int player_start_index, int player_end_index){
    // player_start_index is inclusive and player_end_index us exclusive
    for (int i = player_start_index; i < player_end_index; i++){
        players[i].username = NULL;
        players[i].curr_keyword_count = 
        players[i].round = 0;
        players[i].keywords = (char**)malloc(MAX_NUM_OF_KEYWORD*sizeof(char*));
        for (int j = 0; j < MAX_NUM_OF_KEYWORD; j++){
            players[i].keywords[j] = (char*)malloc(MAX_KEYWORD_LENGTH*sizeof(char));
        }
    }
    return;
}

char* find_username(player_set_t* player_set, int cookie_id){
    if (cookie_id <= player_set->max_size){
        return player_set->players[cookie_id].username;
    }else{
        printf("Invalid Cookie ID, Out of range\n");
    }
    return NULL;
}

void add_username(player_set_t* player_set, int cookie_id, char* username){
    int len =strlen(username);
    player_set->players[cookie_id].username = (char*)malloc(len*sizeof(char));
    strncpy(player_set->players[cookie_id].username,
            username,
            strlen(username));
    char* the_username = player_set->players[cookie_id].username;
    the_username[len] = '\0';
}

void add_keyword(player_set_t* player_set, int cookie_id, char* keyword){
    int len =strlen(keyword);
    int index = player_set->players[cookie_id].curr_keyword_count++;
    strncpy(player_set->players[cookie_id].keywords[index], keyword, len);
    char* the_keyword = player_set->players[cookie_id].keywords[index];
    the_keyword[len] = '\0';
}

void add_cookie(player_set_t* player_set){
    int next_cookie_id = player_set->curr_size;

    update_memory_of_player_set(player_set, next_cookie_id);
    player_set->players[next_cookie_id].cookie = next_cookie_id;
    player_set->curr_size += 1;
}

void update_memory_of_player_set(player_set_t* player_set, int next_cookie_id){
    if (next_cookie_id >= player_set->max_size){
        player_set->max_size *= 2;
        player_set->players
        = realloc(player_set->players, player_set->max_size * sizeof(player_t));
        initialise_players(player_set->players, player_set->max_size/2, player_set->max_size);
    }
}

int is_valid_cookie(player_set_t* player_set, int cookie){
    if (cookie < player_set->curr_size){
        return 1;
    }
    return 0;
}

void print_all_cookies(player_set_t* player_set){
    printf("Summary of cookie and username pairs\n");
    for (int i=0; i < player_set->curr_size; i++){
        printf("Cookie :%d ", player_set->players[i].cookie);
        // printf("%s\n", player_set->player[i].username);
        char* username;
        if (player_set->players[i].username != NULL){
            printf("The username for cookie %d is not null.\n", i);
            username = player_set->players[i].username;
            printf("get username successfully %s\n", username);
        }else{
            printf("The cookie %d has no username\n",i );
            username = "No username provided.";
        }
        printf("Username:%s\n", username);
    }
    printf("Print all player successfully\n");
}

void print_player(player_t* player){
    printf("Player name is %s\n", player->username);
    printf("Player cookie is %d\n", player->cookie);
}

int does_contain_cookie(char* buff, player_set_t* player_set){
    if (strstr(buff, "Cookie: ") != NULL &&
    is_valid_cookie(player_set, atoi(get_cookie(buff)))){
        return 1;
    }
    return 0;
}

char* get_cookie(char* buff){
    if (strstr(buff, "Cookie: ") != NULL){
        char * cookie_curr_pt = strstr(buff, "Cookie: ") + 8;
        char* cookie_id = (char*)malloc(sizeof(char));
        int curr_size = 0;
        int max_size = 1;

        while (isdigit(*cookie_curr_pt)){
            // check whether to expand memory
            if (curr_size == max_size){
                max_size *= 2;
                cookie_id = realloc(cookie_id, max_size*sizeof(char));
            }

            cookie_id[curr_size++] = *cookie_curr_pt;
            cookie_curr_pt++;
        }
        cookie_id[curr_size] = '\0';
        return cookie_id;
    }
    return NULL;
}

// Assume the cookie id is valid, that is it has already be bind to one player
player_t* get_player_by_cookie(player_set_t* player_set, int cookie_id){
    for (int i = 0; i < player_set->curr_size; i++){
        if (player_set->players[i].cookie == cookie_id){
            return &player_set->players[i];
        }
    }
    return NULL;
}

char* parse_and_format_keyword(char* buff){
    char* curr = strstr(buff, "keyword=") + 8;
    char* keyword = (char*)malloc(MAX_KEYWORD_LENGTH*sizeof(char));
    int count = 0;
    while(*curr != KEYWORD_STOP_CHAR){
        keyword[count++] = *curr;
        curr++;
    }
    strncpy(keyword+count, "<br>", 5);
    return keyword;
}

char* get_all_key_words_in_one_string(player_t *player){
    int max_length_of_all_keywords = MAX_KEYWORD_LENGTH*MAX_NUM_OF_KEYWORD;
    int curr_pt = 0;
    char* all_keywords = (char*)malloc(max_length_of_all_keywords*sizeof(char));
    for (int i = 0; i < player->curr_keyword_count; i++){
        int len = strlen(player->keywords[i]);
        strncpy(all_keywords+curr_pt, player->keywords[i], len);
        curr_pt += len;
    }
    all_keywords[curr_pt] = '\0';
    return all_keywords;
}

int does_keyword_match(char* keyword, player_t* other_player){
    char** other_player_keywords = other_player->keywords;
    int num_keywords = other_player->curr_keyword_count;
    for (int i = 0; i < num_keywords; i++){
        if (strcmp(keyword, other_player_keywords[i]) == 0){
            return 1;
        }
    }
    return 0;
}

void free_player_set(player_set_t* player_set){
    for (int i = 0; i < player_set->curr_size; i++){
        free(player_set->players[i].username);
        for (int j = 0; j < player_set->players[i].curr_keyword_count; j++){
            free(player_set->players[i].keywords[j]);
        }
        free(player_set->players[i].keywords);
    }
    free(player_set->players);
    free(player_set);
}

void clear_player_keywords(player_t* player){
    // Free previous keywords
    for (int i = 0; i < player->curr_keyword_count; i++){
        free(player->keywords[i]);
    }
    free(player->keywords);
    player->curr_keyword_count = 0;

    // Allocate new keywords
    player->keywords = (char**)malloc(MAX_NUM_OF_KEYWORD*sizeof(char*));
    for (int j = 0; j < MAX_NUM_OF_KEYWORD; j++){
        player->keywords[j] = (char*)malloc(MAX_KEYWORD_LENGTH*sizeof(char));
    }
}
