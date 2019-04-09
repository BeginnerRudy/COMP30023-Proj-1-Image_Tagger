/* A simple server in the internet domain using TCP
The port number is passed as an argument


 To compile: gcc server.c -o server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


int main(int argc, char **argv)
{
	// sockdf => the welcoming socket
	// newsockfd => the connection socket
	// portno => the port # for wecoming socket
	int sockfd, newsockfd, portno;
	// The server reads characters from the socket connection into this buffer.
	char buffer[256];
	/***
	struct sockaddr_in{
		short	sin_family;
		u_short	sin_port;
		struct	in_addr sin_addr;
		char 	sin_zero[8];
	};
	*/
	struct sockaddr_in serv_addr, cli_addr;
	// clilen stores the size of the address of the client.
	// This is needed for the accept system call
	socklen_t clilen;
	// n is the return calue for the read() and write() calls;
	// i.e. it contains the number of characters read or written.
	int n;

	// Check whether the user pass the port number as an argument
	if (argc < 2) // argc == 0 or 1 => ./server
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	 /* Create TCP socket */
	// This sockect takes 3 args
	// sockect(AddressDomain, SocketType, Protocol)
	// AddressDomain = {AF_UNIX => UnixDomain, AF_INET => InternetDomain}
	// SocketType = {SOCK_STREAM => chars read in stream,
	//				 SOCK_DGRAM =>chars read in chunk, means once a message}
	// Protocol = 0 => (SOCK_DGRAM => UDP) & (SOCK_STREAM => TCP)
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// If we fail to open a socket, the socket() call would return -1 to indicate failure.
	if (sockfd < 0) // Fail to open socket
	{
		perror("ERROR opening socket");
		exit(1);
	}

	// the bzero() call initialize all value in the buffer to 0
	bzero((char *) &serv_addr, sizeof(serv_addr));

	// Assign the port number from comment line argument to portno
	// atoi() => string to integers
	portno = atoi(argv[1]);

	/* Create address we're going to listen on (given port number)
	 - converted to network byte order & any IP address for
	 this machine */

	 // create server address, IP + port number

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);  // store in machine-neutral format

	 /* Bind address to the socket */
	 // in this case, bind() call binds sockfd
	 //with the address of the current host qnd port number on which the server will run
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
			sizeof(serv_addr)) < 0)
	{
		perror("ERROR on binding");
		exit(1);
	}

	/* Listen on socket - means we're ready to accept connections -
	 incoming connection requests will be queued */

	listen(sockfd,5);// 5 => the number of connections that can be waiting

	clilen = sizeof(cli_addr);//

	/* Accept a connection - block until a connection is ready to
	 be accepted. Get back a new file descriptor to communicate on. */

	 // NEW!!!
	newsockfd = accept(	sockfd, (struct sockaddr *) &cli_addr,
						&clilen);

	// handle open new socket failure
	if (newsockfd < 0)
	{
		perror("ERROR on accept");
		exit(1);
	}

	// initialize each value of the buffer to 0
	bzero(buffer,256);

	/* Read characters from the connection,
		then process */

	n = read(newsockfd,buffer,255);

	// handling reading failure
	if (n < 0)
	{
		perror("ERROR reading from socket");
		exit(1);
	}

	// Display the client request on terminal's screen
	printf("Here is the message: %s\n",buffer);

	// write message to the connection socket
	n = write(newsockfd,"I got your message",18);// 18 => The size of the message

	// handling writing failure
	if (n < 0)
	{
		perror("ERROR writing to socket");
		exit(1);
	}

	/* close socket */

	close(sockfd);

	return 0;
}
