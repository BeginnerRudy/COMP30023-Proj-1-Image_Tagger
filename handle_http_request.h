#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "game.h"
#include "response.h"


int is_GET_HOME_PAGE(char* curr);
int is_GET_GAME_PLAYING_PAGE(char* curr);
int is_GET_FAV_ICON(char* curr);
int is_QUIT(char* buff);
int is_SUBMIT_Username(char* buff);
int is_GUESS_Keyword(char* buff);
bool is_socket_closed(int n, int sockfd);

bool handle_GET_request(char* curr, player_set_t* player_set, int sockfd,
    game_info_t* game_info);
bool handle_POST_request(char* curr, player_set_t* player_set, int sockfd,
    game_info_t* game_info);
bool handle_http_request(int sockfd, player_set_t* player_set,
    game_info_t* game_info);
