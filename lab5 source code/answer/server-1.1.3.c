/* Server for 5.1.1-5.1.3 */

#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char ** argv)
{
    int sockfd, newsockfd, portno; // clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    int n;
    char * c;

    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(EXIT_FAILURE);
    }

    /* Create TCP socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));

    portno = atoi(argv[1]);

    /* Create address we're going to listen on (given port number)
     - converted to network byte order & any IP address for this machine */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno); // store in machine-neutral format

    /* Bind address to the socket */
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        exit(EXIT_FAILURE);
    }

    /* Listen on socket - means we're ready to accept connections -
     incoming connection requests will be queued */
    listen(sockfd, 5);

    clilen = sizeof(cli_addr);

    /* Accept a connection - block until a connection is ready to
     be accepted. Get back a new file descriptor to communicate on. */
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
    {
        perror("ERROR on accept");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        /* Read characters from the connection, then process */
        n = read(newsockfd, buffer, 255);
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

        if (!strncmp(buffer, "GOODBYE-CLOSE-TCP", 17))
            break;

        c = buffer;
        while (*c)
        {
            *c = toupper(*c);
            ++c;
        }

        n = write(newsockfd, buffer, n);
        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(EXIT_FAILURE);
        }
    }

    /* close socket */
    close(sockfd);

    return 0;
}
