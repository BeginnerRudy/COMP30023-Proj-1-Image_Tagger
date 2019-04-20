#include "handle_http_request.h"

bool send_body(int sockfd, char* buff, int n, char* page_to_sent){
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

bool send_page_to_user_with_cookie(char* page_to_sent, char* buff,
    int n, int sockfd, int num_bit_add, int cookie_id){
    // get the size of the file
    struct stat st;
    stat(page_to_sent, &st);
    n = sprintf(buff, HTTP_200_FORMAT_WITH_COOKIE, st.st_size + num_bit_add, cookie_id);
    send_body(sockfd, buff, n, page_to_sent);
    return true;
}

bool send_page_to_user_no_cookie(char* page_to_sent, char* buff,
    int n, int sockfd, int num_bit_add){
    // get the size of the file
    struct stat st;
    stat(page_to_sent, &st);
    n = sprintf(buff, HTTP_200_FORMAT, st.st_size + num_bit_add);
    send_body(sockfd, buff, n, page_to_sent);
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

int does_contain_cookie(char* buff, cookie_set_t* cookie_set){
    if (strstr(buff, "Cookie: ") != NULL &&
    is_valid_cookie(cookie_set, atoi(get_cookie(buff)))){
        return 1;
    }
    return 0;
}

char* get_cookie(char* buff){
    if (strstr(buff, "Cookie: ") != NULL){
        char * cookie_curr_pt = strstr(buff, "Cookie: ") + 8;
        char* cookie_id = (char*)malloc(sizeof(char));
        int curr_size = 0;
        int max_size = 1;


        while (strcmp(cookie_curr_pt, "\n") != 0){
            // check whether to expand memory
            if (curr_size == max_size){
                max_size *= 2;
                cookie_id = realloc(cookie_id, max_size*sizeof(char));
            }

            cookie_id[curr_size++] = *cookie_curr_pt;
            cookie_curr_pt++;
        }
        cookie_id[curr_size] = '\0';
        printf("========================================================\n");
        printf("The cookie I get is :%s\n", cookie_id);
        printf("========================================================\n");
        return cookie_id;
    }
    return NULL;
}


bool handle_http_request(int sockfd, cookie_set_t* cookie_set)
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
            bool does_send_cookie = false;
            // GET HOME_PAGE
            if (is_GET_HOME_PAGE(curr)){
                // The user has no cookie
                if (!does_contain_cookie(buff, cookie_set)){
                    page_to_sent = (char*)malloc(sizeof(HOME_PAGE));
                    strncpy(page_to_sent, HOME_PAGE, HOME_PAGE_PATH_LENGTH);
                    does_send_cookie = true;
                // The user does have cookie
                }else{
                    page_to_sent = (char*)malloc(sizeof(MAIN_PAGE));
                    strncpy(page_to_sent, MAIN_PAGE, MAIN_PAGE_PATH_LENGTH);
                }
            // GET GAME_PLAYING_PAGE
            }else if(is_GET_GAME_PLAYING_PAGE(curr)){
                page_to_sent = (char*)malloc(sizeof(GAME_PLAYING_PAGE));
                strncpy(page_to_sent, GAME_PLAYING_PAGE, GAME_PLAYING_PAGE_PATH_LENGTH);
            }else if(is_GET_FAV_ICON(curr)){
                page_to_sent = (char*)malloc(sizeof(FAV_ICON));
                strncpy(page_to_sent, FAV_ICON, FAV_ICON_PATH_LENGTH);
            }

            // send the page which the client request to client
            if (does_send_cookie == true){
                int next_cookie_id = cookie_set->curr_size;
                // Send HTTP header along with cookie to player
                send_page_to_user_with_cookie(page_to_sent, buff, n, sockfd,
                     NOTHING_TO_ADD, next_cookie_id);
                // Add this cookie to the cookie set,
                //this function handles dynamic array
                printf("start to new cookie %d\n", cookie_set->curr_size);
                add_cookie(cookie_set);
                printf("add new cookie %d successfully\n", cookie_set->curr_size - 1);

            }else if (strcmp(page_to_sent, MAIN_PAGE) == 0){
                int curr_cookie = atoi(get_cookie(buff));
                char* username = find_username(cookie_set, curr_cookie);
                int username_length = strlen(username);
                int len_of_thing_to_add = username_length + 4;
                send_page_to_user_no_cookie(page_to_sent, buff, n, sockfd,
                 len_of_thing_to_add);
                 // Add username into the main page
                 if (write(sockfd, "<p/>", 4) < 0)
                 {
                     perror("write");
                     return false;
                 }

                 if (write(sockfd, username, username_length) < 0)
                 {
                     perror("write");
                     return false;
                 }
            }else{
                send_page_to_user_no_cookie(page_to_sent, buff, n, sockfd,
                     NOTHING_TO_ADD);
            }
            free(page_to_sent);
        }
        else if (method == POST)
        {
            // quit POST
            if (is_QUIT(buff)){
                send_page_to_user_no_cookie(GAME_OVER_PAGE, buff, n, sockfd,
                    NOTHING_TO_ADD);
                close(sockfd);
            // username POST
            }else if(is_SUBMIT_Username(buff)){
                // locate the username, it is safe to do so in this sample code, but usually the result is expected to be
                // copied to another buffer using strcpy or strncpy to ensure that it will not be overwritten.
                char * name = strstr(buff, "user=") + 5;
                int name_length = strlen(name);
                int len_of_thing_to_add = name_length + 4;

                // store the username along with cookie ID
                if (get_cookie(buff) == NULL){
                    printf("No cookie provided\n");
                }else{
                    int curr_cookie = atoi(get_cookie(buff));
                    printf("The cookie I got now is %d\n", curr_cookie);
                    printf("The username I got is %s\n", name);
                    add_username(cookie_set, curr_cookie, name);
                }
                // add_username(cookie_set, curr_cookie, username);

                // send the modified Main Page
                send_page_to_user_no_cookie(MAIN_PAGE, buff, n, sockfd,
                    len_of_thing_to_add);
                // Add username into the main page
                if (write(sockfd, "<p/>", 4) < 0)
                {
                    perror("write");
                    return false;
                }

                if (write(sockfd, name, name_length) < 0)
                {
                    perror("write");
                    return false;
                }

            // keyword POST
            }else if(is_GUESS_Keyword(buff)){
                // keyword accepted
                // keyword discard
                // game completed

                send_page_to_user_no_cookie(KEYWORD_ACCEPTED_PAGE, buff, n, sockfd,
                    NOTHING_TO_ADD);
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

    printf("Current max size is %d\n", cookie_set->max_size);
    print_all_cookies(cookie_set);
    return true;
}
