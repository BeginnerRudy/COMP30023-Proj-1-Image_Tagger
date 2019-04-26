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


bool handle_http_request(int sockfd, player_set_t* player_set)
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


    // handle http GET request
    if (method == GET){
        if (is_GET_HOME_PAGE(curr)){
            // The user has no cookie, then send cookie
            if (!does_contain_cookie(buff, player_set)){
                int new_cookie_id = player_set->curr_size;
                add_cookie(player_set);

                // Send HTTP header along with cookie to player
                return send_html_with_cookie(HOME_PAGE, sockfd, new_cookie_id);
            // The user does have cookie
            }else{
                int curr_cookie = atoi(get_cookie(buff));
                char* username = find_username(player_set, curr_cookie);
                return send_html_format(MAIN_PAGE, sockfd, username);
            }
        }else if(is_GET_GAME_PLAYING_PAGE(curr)){
            return send_html(GAME_PLAYING_PAGE, sockfd);
            // TODO make playring_player point to current player
        }else if(is_GET_FAV_ICON(curr)){
            return send_fav_icon(FAV_ICON, sockfd);
        }else{
            return send_404(sockfd);
        }
    }else if(method == POST){
        if(is_QUIT(buff)){
            send_html(GAME_OVER_PAGE, sockfd);
            return false;
        }
        //TODO if there is only one player playing
        //TODO else if there are two players playing
        else if(is_SUBMIT_Username(buff)){
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
                add_username(player_set, curr_cookie, name);
            }
            printf("Add username into cookie successfully\n");

            // send the modified Main Page
            send_html_format(MAIN_PAGE, sockfd, name);
        }else if(is_GUESS_Keyword(buff)){
            // keyword accepted
            // keyword discard
            // game completed
            int cookie_id = atoi(get_cookie(buff));
            char * keyword = parse_and_format_keyword(buff);
            printf("The cookie I got is %d\n", cookie_id);
            add_keyword(player_set, cookie_id, keyword);
            printf("The keyword I got is %s\n",
                get_all_key_words_in_one_string(&player_set->players[cookie_id]));
            send_html_format(KEYWORD_ACCEPTED_PAGE, sockfd,
                get_all_key_words_in_one_string(&player_set->players[cookie_id]));
        }else{
            return send_404(sockfd);
        }
    }
    else{
        return send_404(sockfd);
    }
    printf("Current max size is %d\n", player_set->max_size);
    print_all_cookies(player_set);
    return true;
}

bool handle_GET_request(char* curr, player_set_t* player_set, int sockfd){
    if (is_GET_HOME_PAGE(curr)){
        // The user has no cookie
        if (!does_contain_cookie(curr, player_set)){
            int new_cookie_id = player_set->curr_size;
            add_cookie(player_set);
            // Send HTTP header along with cookie to player as well as the welcome page
            return send_html_with_cookie(HOME_PAGE, sockfd, new_cookie_id);
        // The user does have cookie
        }else{
            int curr_cookie = atoi(get_cookie(curr));
            char* username = find_username(player_set, curr_cookie);
            return send_html_format(MAIN_PAGE, sockfd, username);
        }
    }else if(is_GET_GAME_PLAYING_PAGE(curr)){
        return send_html(GAME_PLAYING_PAGE, sockfd);
        // TODO make playring_player point to current player
    }else if(is_GET_FAV_ICON(curr)){
        return send_fav_icon(FAV_ICON, sockfd);
    }else{
        return send_404(sockfd);
    }
}
