#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>

#define HOME_PAGE "./html_files/1_welcome.html"
#define BUFFER_SIZE 1024

// open welcoming socket and connection socket
int main(int argc, char **argv){
  int welcoming_sockfd,   // The fd for welcoming socket
      connection_sockfd_1,// The fd for connection socket for 1st player
      connection_sockfd_2,// The fd for connection socket for 2nd player
      port_no;// The port number for this server would running on

  //The server reads chars from the socket connection into this buffer.
  char buffer[BUFFER_SIZE];

  /***
	struct sockaddr_in{
		short	    sin_family;
		u_short	  sin_port;
		struct	  in_addr sin_addr;
		char 	    sin_zero[8];
	};
	*/
  struct sockaddr_in serv_addr, cli_addr;

  socklen_t clilen; /** clilen stores the size of the address of the client.
	                   *  This is needed for the accept system call*/

  int n; // The number of bytes of char read or write to socket

  // If user does not provide enough information, tell him/she, then EXIT.
  if (argc < 3){
    fprintf(stderr, "ERROR, please enter <Server_IP> and <port_numerb>\n");
    exit(EXIT_FAILURE);
  }

  // Print out server information
  printf("image_tagger server is now running at IP: %s on port %s\n", argv[1], argv[2]);

  //Create a TCP welcoming socket
  welcoming_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (welcoming_sockfd < 0)
  {
      perror("ERROR opening socket");
      exit(EXIT_FAILURE);
  }

  bzero((char*)&serv_addr, sizeof(serv_addr));

  port_no = atoi(argv[2]);

  /* Create address we're going to listen on (given port number)
   - converted to network byte order & any IP address for this machine */
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port_no); // store in machine-neutral format

  /* Bind address to the socket */
  if (bind(welcoming_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
      perror("ERROR on binding");
      exit(EXIT_FAILURE);
  }

  /* Listen on socket - means we're ready to accept connections -
   incoming connection requests will be queued */
  listen(welcoming_sockfd, 5);

  clilen = sizeof(cli_addr);

  /* Accept a connection - block until a connection is ready to
   be accepted. Get back a new file descriptor to communicate on. */
  connection_sockfd_1 = accept(welcoming_sockfd, (struct sockaddr *)&cli_addr, &clilen);
  if (connection_sockfd_1 < 0)
  {
      perror("ERROR on accept");
      exit(EXIT_FAILURE);
  }

  while (1)
  {
      /* Read characters from the connection, then process */
      n = read(connection_sockfd_1, buffer, 255);
      if (n < 0)
      {
          perror("ERROR reading from socket");
          exit(EXIT_FAILURE);
      }
      if (!n)
      {
          printf("Connection closed unexpectedly\n");
          break;
      }
      buffer[n] = 0;

      printf("%s\n", buffer);

      if (!strncmp(buffer, "GOODBYE-CLOSE-TCP", 17))
          break;

      // Writing HTML response message
      write(connection_sockfd_1, "HTTP/1.1 200 OK\n", 16);
      write(connection_sockfd_1, "Content-length: 600\n", 19);
      write(connection_sockfd_1, "Content-Type: text/html\n\n", 25);

      // directly copy data from the file to the socket
      int filefd = open(HOME_PAGE, O_RDONLY);
      int re = 0;
      // loop until all bytes are sent
      do
      {
          re = sendfile(connection_sockfd_1, filefd, NULL, 2048);
      }
      while (re > 0);

      if (re < 0)
      {
          perror("ERROR sending file");
          exit(EXIT_FAILURE);
      }

      close(filefd);


      // // directly copy data from the file to the socket
      // FILE* html_to_transfer = fopen(HOME_PAGE, "r");
      // if (html_to_transfer == NULL){
      //   printf("Failed to open file\n");
      // }
      //
      // bzero(buffer,BUFFER_SIZE);
      // while (fgets(buffer, BUFFER_SIZE, html_to_transfer) != NULL){
      //   write(connection_sockfd_1, buffer, BUFFER_SIZE);
      //   bzero(buffer,BUFFER_SIZE);
      // }
      // fclose(html_to_transfer);

      printf("home page has been sent\n");
  }

  /* close socket */
  close(welcoming_sockfd);

  return 0;
}
