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
