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


bool handle_http_request(int sockfd, player_set_t* player_set,
    game_info_t* game_info)
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


    // handle http requests depends on method
    if (method == GET){
        return handle_GET_request(curr, player_set, sockfd, game_info);
    }else if(method == POST){
        return handle_POST_request(curr, player_set, sockfd, game_info);
    }
    else{
        return send_404(sockfd);
    }
}

bool handle_GET_request(char* curr, player_set_t* player_set, int sockfd,
    game_info_t* game_info){
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
        // register this player into game
        int cookie_id = atoi(get_cookie(curr));
        player_t* curr_player = get_player_by_cookie(player_set, cookie_id);
        register_into_game(game_info, curr_player);
        curr_player->round++;
        print_game_info(game_info);
        char* image_src = get_img_src(curr_player->round);
        return send_html_format(GAME_PLAYING_PAGE, sockfd, image_src);
    }else if(is_GET_FAV_ICON(curr)){
        return send_fav_icon(FAV_ICON, sockfd);
    }else{
        return send_404(sockfd);
    }
}

bool handle_POST_request(char* curr, player_set_t* player_set, int sockfd,
    game_info_t* game_info){
    if(is_SUBMIT_Username(curr)){
        char * name = strstr(curr, "user=") + 5;
        get_cookie(curr);

        // store the username along with cookie ID
        if (get_cookie(curr) == NULL){
            printf("No cookie provided\n");
            return false;
        }else{
            int curr_cookie = atoi(get_cookie(curr));
            add_username(player_set, curr_cookie, name);
        }

        // send the modified Main Page
        return send_html_format(MAIN_PAGE, sockfd, name);
    }else if (is_QUIT(curr)){
        // If the game is over, send back game over page
        if (game_info->is_game_over == 1){
            game_info->is_game_over = 0;
            game_info->num_active_player = 0;
            clear_player_game_info(game_info, atoi(get_cookie(curr)));
            print_game_info(game_info);
            return send_html(GAME_OVER_PAGE, sockfd);
        // If the game is end, send back game completed game
        }else if(game_info->is_game_end == 1){
            game_info->is_game_end = 0;
            game_info->is_game_over = 1;
            game_info->num_active_player = 0;
            clear_player_game_info(game_info, atoi(get_cookie(curr)));
            print_game_info(game_info);
            return send_html(GAME_OVER_PAGE, sockfd);
        }else if(game_info->num_active_player == 1){
            clear_player_game_info(game_info, atoi(get_cookie(curr)));
            game_info->num_active_player = 0;
            print_game_info(game_info);
            return send_html(GAME_OVER_PAGE, sockfd);
        }else if(game_info->num_active_player == 2){
            game_info->is_game_over = 1;
            clear_player_game_info(game_info, atoi(get_cookie(curr)));
            game_info->num_active_player = 1;
            print_game_info(game_info);
            return send_html(GAME_OVER_PAGE, sockfd);
        }else{
            return send_html(GAME_OVER_PAGE, sockfd);
        }
    }else if(is_GUESS_Keyword(curr)){
        printf("Strating to guessing \n");
        print_game_info(game_info);
        // If the game is over, send back game over page
        if (game_info->is_game_over == 1){
            game_info->is_game_over = 0;
            game_info->num_active_player = 0;
            clear_player_game_info(game_info, atoi(get_cookie(curr)));
            print_game_info(game_info);
            return send_html(GAME_OVER_PAGE, sockfd);
        // If the game is end, send back game completed game
        }else if(game_info->is_game_end == 1){
            game_info->is_game_end = 0;
            game_info->num_active_player = 0;
            clear_player_game_info(game_info, atoi(get_cookie(curr)));
            print_game_info(game_info);
            return send_html(GAME_COMPLETED_PAGE, sockfd);
        }else if(game_info->num_active_player == 1){
            print_game_info(game_info);
            int cookie_id = atoi(get_cookie(curr));
            player_t* curr_player = get_player_by_cookie(player_set, cookie_id);
            char* image_src = get_img_src(curr_player->round);
            return send_html_format(KEYWORD_DISCARDED_PAGE, sockfd, image_src);
        }else if(game_info->num_active_player == 2){
            int cookie_id = atoi(get_cookie(curr));
            char * keyword = parse_and_format_keyword(curr);
            add_keyword(player_set, cookie_id, keyword);
            player_t* another_player = get_another_player(game_info, cookie_id);
            if (does_keyword_match(keyword, another_player)){
                game_info->is_game_end = 1;
                clear_player_game_info(game_info, atoi(get_cookie(curr)));
                game_info->num_active_player = 1;
                print_game_info(game_info);
                return send_html(GAME_COMPLETED_PAGE, sockfd);
            }else{
                print_game_info(game_info);
                int cookie_id = atoi(get_cookie(curr));
                player_t* curr_player = get_player_by_cookie(player_set, cookie_id);
                char* image_src = get_img_src(curr_player->round);
                return send_html_format(KEYWORD_ACCEPTED_PAGE, sockfd, image_src,
                    get_all_key_words_in_one_string(&player_set->players[cookie_id]));
            }
        }else{
            return send_404(sockfd);
        }
    }else{
        return send_404(sockfd);
    }

    return false;
}
