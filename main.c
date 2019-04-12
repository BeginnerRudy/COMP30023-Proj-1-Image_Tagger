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

#include "http_request.h"
#include "html_pages.h"
#define BUFFER_SIZE 1024

void my_send_file(char* file_path, int sockfd){
  // directly copy data from the file to the socket
  int filefd = open(file_path, O_RDONLY);
  int re = 0;
  // loop until all bytes are sent
  do
  {
      re = sendfile(sockfd, filefd, NULL, 2048);
  }
  while (re > 0);

  if (re < 0)
  {
      perror("ERROR sending file");
      exit(EXIT_FAILURE);
  }
  close(filefd);
}
void my_select(fd_set* readfds){
  if (select(FD_SETSIZE, readfds, NULL, NULL, NULL) < 0)
  {
      perror("select");
      exit(EXIT_FAILURE);
  }
}
void is_command_line_argument_valid(char* ip, char* port_number, int argc){
  // If user does not provide enough information, tell him/she, then EXIT.
  if (argc < 3){
    fprintf(stderr, "ERROR please enter <Server_IP> and <port_numerb>\n");
    exit(EXIT_FAILURE);
  }

  // Print out server information
  printf("image_tagger server is now running at IP: %s on port %s\n", ip, port_number);
}
void create_welcoming_socket(int* welcoming_sockfd, char* port_number,
   int* port_no, struct sockaddr_in* serv_addr){
  *welcoming_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (*welcoming_sockfd < 0)
  {
      perror("ERROR opening socket");
      exit(EXIT_FAILURE);
  }

  bzero((char*)serv_addr, sizeof(*serv_addr));

  *port_no = atoi(port_number);

  /* Create address we're going to listen on (given port number)
   - converted to network byte order & any IP address for this machine */
  serv_addr->sin_family = AF_INET;
  serv_addr->sin_addr.s_addr = INADDR_ANY;
  serv_addr->sin_port = htons(*port_no); // store in machine-neutral format

  /* Bind address to the socket */
  if (bind(*welcoming_sockfd, (struct sockaddr *)serv_addr, sizeof(*serv_addr)) < 0)
  {
      perror("ERROR on binding");
      exit(EXIT_FAILURE);
  }
}
void initialise_fd_set(fd_set *masterfds, int* maxfd, int welcoming_sockfd){
  FD_ZERO(masterfds);
  FD_SET(welcoming_sockfd, masterfds);
  // record the maximum socket number
  *maxfd = welcoming_sockfd;
}
void three_way_handshaking(int welcoming_sockfd, fd_set* masterfds, int* maxfd){
  printf("welcoming_sockfd get active\n");
  struct sockaddr_in cliaddr;
  socklen_t clilen = sizeof(cliaddr);
  int newsockfd = accept(welcoming_sockfd, (struct sockaddr *)&cliaddr, &clilen);
  if (newsockfd < 0)
      perror("accept");
  else{
      // add the socket to the set
      FD_SET(newsockfd, masterfds);
      // update the maximum tracker
      if (newsockfd > *maxfd)
          *maxfd = newsockfd;
      // print out the IP and the socket number
      char ip[INET_ADDRSTRLEN];
      printf(
          "player %d connected from %s\n",
          // convert to human readable string
          newsockfd,
          inet_ntop(cliaddr.sin_family, &cliaddr.sin_addr, ip, INET_ADDRSTRLEN)

      );
  }
}
void http_server(fd_set masterfds, int *maxfd, int welcoming_sockfd){
  int count = 1;
  while (1)
  {
    printf("++++++++++++++++++++++++++++Round %d+++++++++++++++++++++++++\n", count++);
      // monitor file descriptors
      fd_set readfds = masterfds;
      if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0)
      {
          perror("select");
          exit(EXIT_FAILURE);
      }

      // loop all possible descriptor
      for (int i = 0; i <= *maxfd; ++i){
          // determine if the current file descriptor is active
          printf("Welcoming socket id is %d\n", welcoming_sockfd);
          if (FD_ISSET(i, &readfds)){
              printf("%d\n", i);

              // create new socket if there is new incoming connection request
              if (i == welcoming_sockfd){
                printf("WWWWWWWWWWelcome!\n");
                three_way_handshaking(welcoming_sockfd, &masterfds, maxfd);
              }
              // a request is sent from the client
              else if (!handle_http_request(i))
              {
                  close(i);
                  FD_CLR(i, &masterfds);
              }
          }
        }
  }
}

int main(int argc, char **argv){


  int welcoming_sockfd,   // The fd for welcoming socket
      port_no;// The port number for this server would running on

  /***
	struct sockaddr_in{
		short	    sin_family;
		u_short	  sin_port;
		struct	  in_addr sin_addr;
		char 	    sin_zero[8];
	};
	*/
  struct sockaddr_in serv_addr;


  fd_set masterfds;
  int maxfd;

  // If user does not provide enough information, report it to him/she, then EXIT.
  is_command_line_argument_valid(argv[1], argv[2], argc);

  //Create a TCP welcoming socket
  create_welcoming_socket(&welcoming_sockfd, argv[2], &port_no, &serv_addr);

  /* Listen on socket - means we're ready to accept connections -
   incoming connection requests will be queued */
  listen(welcoming_sockfd, 5);

  // initialise an active file descriptors set
  initialise_fd_set(&masterfds, &maxfd, welcoming_sockfd);

  // running the http Server
  http_server(masterfds, &maxfd, welcoming_sockfd);


  return 0;
}
