#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_MAX_COOKIE_NUM 1000
#define MAX_USERNAME_LEN 20
typedef struct{
    int cookie; // The cookie identifier
    char* username;// The username for the player has this cookie
}cookie_t;

typedef struct{
    cookie_t* cookies;
    int curr_size;
    int max_size;
}cookie_set_t;

cookie_set_t* create_cookie_set();
char* find_username(cookie_set_t* cookie_set, int cookie_id);
void add_username(cookie_set_t* cookie_set,
    int cookie_id, char* username);
void update_memory_of_cookie_set(cookie_set_t* cookie_set,
    int next_cookie_id);
void add_cookie(cookie_set_t* cookie_set);
int is_valid_cookie(cookie_set_t* cookie_set, int cookie);
void print_all_cookies(cookie_set_t* cookie_set);
