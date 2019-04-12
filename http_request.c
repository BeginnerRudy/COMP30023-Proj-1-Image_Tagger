#include "http_request.h"
bool handle_http_request(int sockfd)
{
    // try to read the request
    char buff[2049];
    int n = read(sockfd, buff, 2049);
    if (n <= 0)
    {
        if (n < 0)
            perror("read");
        else
            printf("socket %d close the connection\n", sockfd);
        return false;
    }

    // terminate the string
    buff[n] = 0;

    // display the HTTP request message
    printf("player %d sends a request:\n", sockfd);
    printf("%s\n", buff);

    char * curr = buff;

    // parse the method
    METHOD method = UNKNOWN;
    if (strncmp(curr, "GET ", 4) == 0)
    {
        curr += 4;
        method = GET;
    }
    else if (strncmp(curr, "POST ", 5) == 0)
    {
        curr += 5;
        method = POST;
    }
    else if (write(sockfd, HTTP_400, HTTP_400_LENGTH) < 0)
    {
        perror("write");
        return false;
    }

    // sanitise the URI
    while (*curr == '.' || *curr == '/')
        ++curr;
    // assume the only valid request URI is "/" but it can be modified to accept more files
    if (*curr == ' '){
        if (method == GET)
        {
            // get the size of the file
            struct stat st;
            stat(HOME_PAGE, &st);
            n = sprintf(buff, HTTP_200_FORMAT, st.st_size);
            // send the header first
            if (write(sockfd, buff, n) < 0)
            {
                perror("write");
                return false;
            }
            // send the file
            int filefd = open(HOME_PAGE, O_RDONLY);
            do
            {
                n = sendfile(sockfd, filefd, NULL, 2048);
            }
            while (n > 0);
            if (n < 0)
            {
                perror("sendfile");
                close(filefd);
                return false;
            }
            close(filefd);
        }
        else if (method == POST)
        {
            // locate the username, it is safe to do so in this sample code, but usually the result is expected to be
            // copied to another buffer using strcpy or strncpy to ensure that it will not be overwritten.
            char * username = strstr(buff, "user=") + 5;
            int username_length = strlen(username);
            // the length needs to include the ", " before the username
            long added_length = username_length + 2;

            // get the size of the file
            struct stat st;
            stat(MAIN_PAGE, &st);
            // increase file size to accommodate the username
            long size = st.st_size + added_length;
            n = sprintf(buff, HTTP_200_FORMAT, size);
            // send the header first
            if (write(sockfd, buff, n) < 0)
            {
                perror("write");
                return false;
            }
            // read the content of the HTML file
            int filefd = open(MAIN_PAGE, O_RDONLY);
            n = read(filefd, buff, 2048);
            if (n < 0)
            {
                perror("read");
                close(filefd);
                return false;
            }
            close(filefd);
            // move the trailing part backward
            int p1, p2;
            for (p1 = size - 1, p2 = p1 - added_length; p1 >= size - 25; --p1, --p2)
                buff[p1] = buff[p2];
            ++p2;
            // put the separator
            buff[p2++] = ',';
            buff[p2++] = ' ';
            // copy the username
            strncpy(buff + p2, username, username_length);
            if (write(sockfd, buff, size) < 0)
            {
                perror("write");
                return false;
            }
        }
        else
            // never used, just for completeness
            fprintf(stderr, "no other methods supported");
    }
    // send 404
    else if (write(sockfd, HTTP_404, HTTP_404_LENGTH) < 0)
    {
        perror("write");
        return false;
    }

    return true;
}
