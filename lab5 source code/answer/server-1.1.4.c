/* Server for 5.1.4 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "usage %s port path_to_file\n", argv[0]);
        return 0;
    }

    // create TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }

    // create and initialise address we will listen on
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    int port = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    // bind address to socket
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        exit(EXIT_FAILURE);
    }

    // listen on the socket
    listen(sockfd, 5);

    // accept connection
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
    {
        perror("ERROR on accept");
        exit(EXIT_FAILURE);
    }

    // define buffer
    char buff[2048];

    // open file for writing
    int filefd = open(argv[2], O_WRONLY | O_CREAT, 0600);

    // read from socket and write to file
    int n;
    while (1)
    {
        n = read(newsockfd, buff, 2048);
        if (n == 0)
        {
            break;
        }
        if (n < 0)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }

        n = write(filefd, buff, n);
        if (n < 0)
        {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    close(filefd);
    close(newsockfd);
    close(sockfd);

    return 0;
}
