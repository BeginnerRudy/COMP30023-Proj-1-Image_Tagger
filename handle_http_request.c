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

bool parse_method(char** curr, METHOD* method, int sockfd){
    // parse the method
    if (strncmp(*curr, "GET ", 4) == 0)
    {
        *curr += 4;
        *method = GET;
    }
    else if (strncmp(*curr, "POST ", 5) == 0)
    {
        *curr += 5;
        *method = POST;
    }
    else if (write(sockfd, HTTP_400, HTTP_400_LENGTH) < 0)
    {
        perror("write");
        return false;
    }
    return true;
}


bool handle_http_request(int sockfd, cookie_set_t* cookie_set, player_t* player_set)
{
    // try to read the request
    char buff[2049];
    int n = read(sockfd, buff, 2049);
    // terminate the string
    buff[n] = 0;
    printf("%s\n", buff);
    if (is_socket_closed(n, sockfd)){
        return false;
    }

    char * curr = buff;
    char** ptr = &curr;
    METHOD method = UNKNOWN;

    if (!parse_method(ptr, &method, sockfd)){
        printf("Parsing Error \n");
        return false;
    }


    // assume the only valid request URI is "/" but it can be modified to accept more files
    if (method == GET){
        if (is_GET_HOME_PAGE(curr)){
            // The user has no cookie, then send cookie
            if (!does_contain_cookie(buff, cookie_set)){
                int next_cookie_id = cookie_set->curr_size;
                // Send HTTP header along with cookie to player
                send_html_with_cookie(HOME_PAGE, buff, sockfd, next_cookie_id);
                // Add this cookie to the cookie set,
                //this function handles dynamic array

                add_cookie(cookie_set);
                // create player infro


            // The user does have cookie
            }else{
                int curr_cookie = atoi(get_cookie(buff));
                char* username = find_username(cookie_set, curr_cookie);
                send_html_format(MAIN_PAGE, buff, sockfd, username);
            }
        }else if(is_GET_GAME_PLAYING_PAGE(curr)){
            send_html(GAME_PLAYING_PAGE, buff, sockfd);
        }else if(is_GET_FAV_ICON(curr)){
            send_fav_icon(FAV_ICON, sockfd);
        }else{
            send_404(sockfd);
        }
    }else if(method == POST){
        if(is_QUIT(buff)){
            send_html(GAME_OVER_PAGE, buff, sockfd);
            return false;
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
            send_html_format(MAIN_PAGE, buff, sockfd, name);
        }else if(is_GUESS_Keyword(buff)){
            // keyword accepted
            // keyword discard
            // game completed
            get_cookie(buff);
            int cookie_id = atoi(buff);
            char * keyword = strstr(buff, "keyword=") + 8;
            add_keyword(keyword, &player_set[cookie_id]);
            printf("The keyword I get is %s\n",keyword );

            send_html(KEYWORD_ACCEPTED_PAGE, buff, sockfd);
        }else{
            send_404(sockfd);
        }
    }
    else if (write(sockfd, HTTP_404, HTTP_404_LENGTH) < 0)
    {
        perror("write");
        return false;
    }
    printf("Current max size is %d\n", cookie_set->max_size);
    print_all_cookies(cookie_set);
    return true;
}
