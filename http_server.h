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

#include "handle_http_request.h"


void my_select(fd_set* readfds);
void is_command_line_argument_valid(char* ip, char* port_number, int argc);
void create_welcoming_socket(int* welcoming_sockfd, char* port_number,
   int* port_no, struct sockaddr_in* serv_addr);
void initialise_fd_set(fd_set *masterfds, int* maxfd, int welcoming_sockfd);
void handle_connection_request(int welcoming_sockfd, fd_set* masterfds, int* maxfd);
void handle_all_incoming_request(fd_set masterfds,
    int *maxfd, int welcoming_sockfd, cookie_set_t* cookie_set, player_t* player_set);
void http_server(int argc, char* argv[], player_t* player_set);
