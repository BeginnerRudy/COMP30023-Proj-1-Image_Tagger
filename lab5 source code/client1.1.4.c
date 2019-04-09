/* A simple client program for server.c

   To compile: gcc client.c -o client

   To run: start the server, then the client */


#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#define TERMINATION "GOODBYE-CLOSE-TCP"

int main(int argc, char ** argv)
{
//========================Declaration===========================================
    int sockfd, portno, n, filefd;
    struct sockaddr_in serv_addr;// The address of server attempts to communicate
    /**
    struct  hostent{
            char    *h_name; => official name of host
            chars   **h_aliases; => alias list
            int     h_addrtype; host address type
            int     h_length; length of address
            char    **h_addr_list; list of addresses from name server
    #define h_addr  h_addr_list[0]  address, for backward compatibility
    };
    */
    struct hostent * server;

    char buffer[256]; // The buffer that clinet used to read.

//=============================initialize======================================
    // Error handling for incorrect input from the command line
    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    // Assign the port number of the server processing running
    portno = atoi(argv[2]);


    /* Translate host name into peer's IP address ;
     * This is name translation service by the operating system
     */
    server = gethostbyname(argv[1]); // like DNS???

    // failure handling
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    /* Building data structures for socket */

    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy(server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(portno);

    /* Create TCP socket -- active open
    * Preliminary steps: Setup: creation of active open socket
    */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(0);
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(0);
    }

    while (1){
        /* Do processing
        */
        printf("If your want to terminate this session, enter GOODBYE-CLOSE-TCP\n");
        printf("Please enter the txt file path: ");

        bzero(buffer, 256);

        fgets(buffer, 255, stdin);

        if (strncmp(buffer, TERMINATION, 17) == 0){
            printf("=====TCP Connection Closed=====\n");
            break;
        }

        //======================COPY===============================

        // directly copy data from the file to the socket
        filefd = open(buffer, O_RDONLY);
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

//========================================================================
        bzero(buffer, 256);

        n = read(sockfd, buffer, 255);

        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(0);
        }

        printf("%s\n", buffer);
    }
    close(filefd);
    close(sockfd);
    return 0;
}
