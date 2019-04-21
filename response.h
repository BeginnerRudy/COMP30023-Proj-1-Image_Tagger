#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "html_pages.h"

// constants
static char const * const HTTP_200_FORMAT_WITH_COOKIE = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: %ld\r\n\
Set-cookie: %d\r\n\r\n";
static char const * const HTTP_200_FORMAT = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: %ld\r\n\r\n";
static char const * const HTTP_400 = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
static int const HTTP_400_LENGTH = 47;
static char const * const HTTP_404 = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
static int const HTTP_404_LENGTH = 45;

// represents the types of method
typedef enum
{
    GET,
    POST,
    UNKNOWN
} METHOD;

#define NOTHING_TO_ADD 0

char* my_readfile(char* page_to_send);
char* prepare_html_format(int* n, char* page_to_send, const char* string,...);
bool send_html_format(char* page_to_send, char* buff,
    int n, int sockfd, const char* string,...);
bool send_page_to_user_with_cookie(char* page_to_sent,
    char* buff, int n, int sockfd, int num_bit_add,
    int cookie_id);
bool send_page_to_user_no_cookie(char* page_to_sent,
    char* buff, int n, int sockfd, int num_bit_add);
bool send_body(int sockfd, char* buff,
    int n, char* page_to_sent);
bool mysendfile(int sockfd, char* buff, int n);
bool send_html(char* page_to_send, char* buff, int n, int sockfd);
bool send_html_with_cookie(char* page_to_send, char* buff, int n, int sockfd,
    int cookie_id);
bool send_fav_icon(char* page_to_send, int sockfd);
bool send_404(int sockfd);
