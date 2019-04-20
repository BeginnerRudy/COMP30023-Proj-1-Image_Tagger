#include "cookie.h"
cookie_set_t* create_cookie_set(){
    //Cookie, username initilization
    cookie_set_t* cookie_set = (cookie_set_t*)malloc(sizeof(cookie_set_t));
    cookie_set->curr_size = 0;
    cookie_set->max_size = INITIAL_MAX_COOKIE_NUM;
    cookie_set->cookies =
    (cookie_t*)malloc(INITIAL_MAX_COOKIE_NUM*sizeof(cookie_t));
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
    int len =strlen(username);
    cookie_set->cookies[cookie_id].username = (char*)malloc(len*sizeof(char));
    strncpy(cookie_set->cookies[cookie_id].username,
            username,
            strlen(username));
    char* the_username = cookie_set->cookies[cookie_id].username;
    the_username[len] = '\0';
}

void add_cookie(cookie_set_t* cookie_set){
    int next_cookie_id = cookie_set->curr_size;

    update_memory_of_cookie_set(cookie_set, next_cookie_id);
    cookie_set->cookies[next_cookie_id].cookie = next_cookie_id;
    cookie_set->curr_size += 1;
    for (int i=0; i < cookie_set->max_size; i++){
        printf("The address of cookie_set->cookies[%d] is %ld\n",
         i, (cookie_set->cookies[i]));
    }

    for (int i=0; i < cookie_set->max_size; i++){
        printf("The address of cookie_set->cookies[%d].username is %ld\n",
         i, (cookie_set->cookies[i].username));
    }
}

void update_memory_of_cookie_set(cookie_set_t* cookie_set, int next_cookie_id){
    if (next_cookie_id >= cookie_set->max_size){
        cookie_set->max_size *= 2;
        cookie_set->cookies
        = realloc(cookie_set->cookies, cookie_set->max_size * sizeof(cookie_t));
        printf("Realloc successful\n" );
    }
}

int is_valid_cookie(cookie_set_t* cookie_set, int cookie){
    if (cookie < cookie_set->curr_size){
        return 1;
    }
    return 0;
}

void print_all_cookies(cookie_set_t* cookie_set){
    printf("Summary of cookie and username pairs\n");
    for (int i=0; i < cookie_set->curr_size; i++){
        printf("Cookie :%d ", cookie_set->cookies[i].cookie);
        // printf("%s\n", cookie_set->cookies[i].username);
        char* username;
        if (cookie_set->cookies[i].username != NULL){
            printf("The username for cookie %d is not null.\n", i);
            username = cookie_set->cookies[i].username;
            printf("get username successfully %s\n", username);
        }else{
            printf("The cookie %d has no username\n",i );
            username = "No username provided.";
        }
        printf("Username:%s\n", username);
    }
    printf("Print all cookies successfully\n");
}
