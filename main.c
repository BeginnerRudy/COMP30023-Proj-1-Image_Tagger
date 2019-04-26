#include <stdio.h>
#include "http_server.h"

#define MAX_PLAYER_HOLD 100

int main(int argc, char **argv){
    // run the http web game server
    http_server(argc, argv);
    return 0;
}
