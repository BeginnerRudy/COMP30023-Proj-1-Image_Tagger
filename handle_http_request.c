#include "handle_http_request.h"

bool send_page_to_user(char* page_to_sent, char* buff, int n, int sockfd){
    // get the size of the file
    struct stat st;
    stat(page_to_sent, &st);
    n = sprintf(buff, HTTP_200_FORMAT_WITH_COOKIE, st.st_size, 0);
    // send the header first
    if (write(sockfd, buff, n) < 0)
    {
        perror("write");
        return false;
    }

    // send the file
    int filefd = open(page_to_sent, O_RDONLY);
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
    return true;
}

int is_GET_HOME_PAGE(char* curr){
    if (strncmp(curr, "/ ",2) == 0){
        return 1;
    }
    return 0;
}

int is_GET_GAME_PLAYING_PAGE(char* curr){
    if (strncmp(curr, "/?start=Start", 13) == 0){
        return 1;
    }
    return 0;
}

int is_GET_FAV_ICON(char* curr){
    if (strncmp(curr, "/favicon.ico", 12) == 0){
        return 1;
    }
    return 0;
}

int is_QUIT(char* buff){
    if (strstr(buff, "quit=") != NULL){
        return 1;
    }
    return 0;
}

int is_SUBMIT_Username(char* buff){
    if (strstr(buff, "user=") != NULL){
        return 1;
    }
    return 0;
}

int is_GUESS_Keyword(char* buff){
    if (strstr(buff, "keyword=") != NULL){
        return 1;
    }
    return 0;
}

bool handle_http_request(int sockfd)
{
    // try to read the request
    char buff[2049];
    int n = read(sockfd, buff, 2049);
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


    // assume the only valid request URI is "/" but it can be modified to accept more files
    if (1){
        if (method == GET)
        {
            char* page_to_sent;
            // GET HOME_PAGE
            if (is_GET_HOME_PAGE(curr)){
                page_to_sent = (char*)malloc(sizeof(HOME_PAGE));
                strncpy(page_to_sent, HOME_PAGE, HOME_PAGE_PATH_LENGTH);
            // GET GAME_PLAYING_PAGE
            }else if(is_GET_GAME_PLAYING_PAGE(curr)){
                page_to_sent = (char*)malloc(sizeof(GAME_PLAYING_PAGE));
                strncpy(page_to_sent, GAME_PLAYING_PAGE, GAME_PLAYING_PAGE_PATH_LENGTH);
            }else if(is_GET_FAV_ICON(curr)){
                page_to_sent = (char*)malloc(sizeof(FAV_ICON));
                strncpy(page_to_sent, FAV_ICON, FAV_ICON_PATH_LENGTH);
            }

            printf("%s\n", page_to_sent);
            // send the page which the client request to client
            send_page_to_user(page_to_sent, buff, n, sockfd);
            free(page_to_sent);
        }
        else if (method == POST)
        {
            // quit POST
            if (is_QUIT(buff)){
            // username POST
            }else if(is_SUBMIT_Username(buff)){
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
            // keyword POST
            }else if(is_GUESS_Keyword(buff)){

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
