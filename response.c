#include "response.h"

char* prepare_html_format(int* n, char* page_to_send, const char* string,...){
    char* html = (char*)malloc(2049*sizeof(char));

    char * buffer = 0;
    long length;
    FILE * f = fopen (page_to_send, "rb");

    if (f)
    {
      fseek (f, 0, SEEK_END);
      length = ftell (f);
      fseek (f, 0, SEEK_SET);
      buffer = malloc (length);
      if (buffer)
      {
        fread (buffer, 1, length, f);
      }
      fclose (f);
    }

    if (buffer)
    {
      // start to process your data / extract strings here...
      *n = sprintf(html, buffer, string);
    }
    return html;
}

bool send_html_format(char* page_to_send, char* buff,
    int n, int sockfd, const char* string,...){
    int len_html;
    char* html = prepare_html_format(&len_html, page_to_send, string);
    n = sprintf(buff, HTTP_200_FORMAT, len_html);
    // send the header first
    if (write(sockfd, buff, n) < 0)
    {
        perror("write");
        return false;
    }
    printf("%s\n", buff);
    // send the body then
    if (write(sockfd, html, len_html) < 0)
    {
        perror("write");
        return false;
    }
    printf("%s\n", html);
    return true;
}


bool send_body(int sockfd, char* buff, int n, char* page_to_send){
    // send the header first
    if (write(sockfd, buff, n) < 0)
    {
        perror("write");
        return false;
    }

    // send the file
    int filefd = open(page_to_send, O_RDONLY);
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

bool send_page_to_user_with_cookie(char* page_to_send, char* buff,
    int n, int sockfd, int num_bit_add, int cookie_id){
    // get the size of the file
    struct stat st;
    stat(page_to_send, &st);
    n = sprintf(buff, HTTP_200_FORMAT_WITH_COOKIE, st.st_size + num_bit_add, cookie_id);
    send_body(sockfd, buff, n, page_to_send);
    return true;
}

bool send_page_to_user_no_cookie(char* page_to_send, char* buff,
    int n, int sockfd, int num_bit_add){
    // get the size of the file
    struct stat st;
    stat(page_to_send, &st);
    n = sprintf(buff, HTTP_200_FORMAT, st.st_size + num_bit_add);
    send_body(sockfd, buff, n, page_to_send);
    return true;
}
