/* Client for 5.1.4 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "usage %s hostname port path_to_file\n", argv[0]);
        return 0;
    }

    // find host
    struct hostent * server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(EXIT_FAILURE);
    }

    // get port number
    int port = atoi(argv[2]);

    // create and initialise server address
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
        server->h_addr_list[0], &serv_addr.sin_addr.s_addr, server->h_length
    );
    serv_addr.sin_port = htons(port);

    // create socket and connect to the server
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(EXIT_FAILURE);
    }
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(EXIT_FAILURE);
    }

    // directly copy data from the file to the socket
    int filefd = open(argv[3], O_RDONLY);
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
    close(sockfd);

    return 0;
}
