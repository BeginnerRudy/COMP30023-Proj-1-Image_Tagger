#include <stdio.h>
#include "http_server.h"

#define MAX_PLAYER_HOLD 100

int main(int argc, char **argv){
    player_t* player_set = (player_t*)malloc(MAX_PLAYER_HOLD*sizeof(player_t));
    // run the http web game server
    http_server(argc, argv, player_set);
    return 0;
}
