#include "cookie.h"
cookie_set_t* create_cookie_set(){
    //Cookie, username initilization
    cookie_set_t* cookie_set = (cookie_set_t*)malloc(sizeof(cookie_set_t*));
    cookie_set->curr_size = 0;
    cookie_set->max_size = INITIAL_MAX_COOKIE_NUM;
    cookie_set->cookies =
    (cookie_t*)malloc(INITIAL_MAX_COOKIE_NUM*sizeof(cookie_t*));

    return cookie_set;
}

char* find_username(cookie_set_t* cookie_set, int cookie_id){
    if (cookie_id <= cookie_set->max_size){
        return cookie_set->cookies[cookie_id].username;
    }else{
        printf("Invalid Cookie ID, Out of range\n");
    }
    return NULL;
}

void add_username(cookie_set_t* cookie_set, int cookie_id, char* username){
    strncpy(cookie_set->cookies[cookie_id].username,
            username,
            strlen(username));
}

void add_cookie(cookie_set_t* cookie_set){
    int next_cookie_id = cookie_set->curr_size;

    update_memory_of_cookie_set(cookie_set, next_cookie_id);
    cookie_set->cookies[next_cookie_id].cookie = next_cookie_id;
    cookie_set->curr_size += 1;
}

void update_memory_of_cookie_set(cookie_set_t* cookie_set, int next_cookie_id){
    if (next_cookie_id >= cookie_set->max_size){
        cookie_set->max_size *= 2;
        cookie_set->cookies = realloc(cookie_set->cookies,
            cookie_set->max_size*sizeof(cookie_t*));
    }
}
