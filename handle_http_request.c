#include "handle_http_request.h"

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

bool is_socket_closed(int n, int sockfd){
    if (n <= 0)
    {
        if (n < 0)
            perror("read");
        else
            printf("socket %d close the connection\n", sockfd);
        return true;
    }
    return false;
}

bool parse_method(char* curr, METHOD* method, int sockfd){
    // parse the method
    if (strncmp(curr, "GET ", 4) == 0)
    {
        curr += 4;
        *method = GET;
    }
    else if (strncmp(curr, "POST ", 5) == 0)
    {
        curr += 5;
        *method = POST;
    }
    else if (write(sockfd, HTTP_400, HTTP_400_LENGTH) < 0)
    {
        perror("write");
        return false;
    }
    return true;
}

bool handle_http_request(int sockfd, cookie_set_t* cookie_set)
{
    // try to read the request
    char buff[2049];
    int n = read(sockfd, buff, 2049);
    // terminate the string
    buff[n] = 0;

    if (is_socket_closed(n, sockfd)){
        return false;
    }



    // display the HTTP request message
    printf("player %d sends a request:\n", sockfd);
    printf("%s\n", buff);

    char * curr = buff;
    METHOD method = UNKNOWN;

    // parse the method
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

    // if (!parse_method(curr, &method, sockfd)){
    //     printf("Parsing S........\n");
    //     printf("Parsing........\n");
    //     return false;
    // }
    // assume the only valid request URI is "/" but it can be modified to accept more files
    if (1){
        if (method == GET)
        {
            char* page_to_send;
            bool does_send_cookie = false;
            // GET HOME_PAGE
            if (is_GET_HOME_PAGE(curr)){
                // The user has no cookie
                if (!does_contain_cookie(buff, cookie_set)){
                    page_to_send = (char*)malloc(sizeof(HOME_PAGE));
                    strncpy(page_to_send, HOME_PAGE, HOME_PAGE_PATH_LENGTH);
                    does_send_cookie = true;
                // The user does have cookie
                }else{
                    page_to_send = (char*)malloc(sizeof(MAIN_PAGE));
                    strncpy(page_to_send, MAIN_PAGE, MAIN_PAGE_PATH_LENGTH);
                }
            // GET GAME_PLAYING_PAGE
            }else if(is_GET_GAME_PLAYING_PAGE(curr)){
                page_to_send = (char*)malloc(sizeof(GAME_PLAYING_PAGE));
                strncpy(page_to_send, GAME_PLAYING_PAGE, GAME_PLAYING_PAGE_PATH_LENGTH);
            }else if(is_GET_FAV_ICON(curr)){
                page_to_send = (char*)malloc(sizeof(FAV_ICON));
                strncpy(page_to_send, FAV_ICON, FAV_ICON_PATH_LENGTH);
            }

            // send the page which the client request to client
            if (does_send_cookie == true){
                int next_cookie_id = cookie_set->curr_size;
                // Send HTTP header along with cookie to player
                send_page_to_user_with_cookie(page_to_send, buff, n, sockfd,
                     NOTHING_TO_ADD, next_cookie_id);
                // Add this cookie to the cookie set,
                //this function handles dynamic array
                printf("start to new cookie %d\n", cookie_set->curr_size);
                add_cookie(cookie_set);
                printf("add new cookie %d successfully\n", cookie_set->curr_size - 1);

            }else if (strcmp(page_to_send, MAIN_PAGE) == 0){
                int curr_cookie = atoi(get_cookie(buff));
                char* username = find_username(cookie_set, curr_cookie);

                send_html_format(page_to_send, buff,
                    n, sockfd, username);
            }else{
                send_page_to_user_no_cookie(page_to_send, buff, n, sockfd,
                     NOTHING_TO_ADD);
            }
            free(page_to_send);
        }
        else if (method == POST)
        {
            // quit POST
            if (is_QUIT(buff)){
                send_page_to_user_no_cookie(GAME_OVER_PAGE, buff, n, sockfd,
                    NOTHING_TO_ADD);
                return false;
            // username POST
            }else if(is_SUBMIT_Username(buff)){
                // locate the username, it is safe to do so in this sample code, but usually the result is expected to be
                // copied to another buffer using strcpy or strncpy to ensure that it will not be overwritten.
                char * name = strstr(buff, "user=") + 5;

                get_cookie(buff);
                printf("get_cookie() successfully\n");

                // store the username along with cookie ID
                if (get_cookie(buff) == NULL){
                    printf("No cookie provided\n");
                }else{
                    printf("Cookie provided.\n");
                    int curr_cookie = atoi(get_cookie(buff));
                    printf("The cookie I got now is %d\n", curr_cookie);
                    printf("The username I got is %s\n", name);
                    add_username(cookie_set, curr_cookie, name);
                }
                printf("Add username into cookie successfully\n");

                // send the modified Main Page
                // send the modified Main Page
                send_html_format(MAIN_PAGE, buff, n, sockfd, name);

            // keyword POST
            }else if(is_GUESS_Keyword(buff)){
                // keyword accepted
                // keyword discard
                // game completed

                send_page_to_user_no_cookie(KEYWORD_ACCEPTED_PAGE, buff, n, sockfd,
                    NOTHING_TO_ADD);
            }

        }
        else{
            // never used, just for completeness
            fprintf(stderr, "no other methods supported");
            return false;
        }
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
