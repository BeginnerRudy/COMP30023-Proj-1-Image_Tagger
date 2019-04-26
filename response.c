#include "response.h"

char* my_readfile(char* page_to_send){
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
    return buffer;
}

bool mysendfile(int sockfd, char* buff, int n){
    if (write(sockfd, buff, n) < 0)
    {
        perror("write");
        return false;
    }
    return true;
}

char* prepare_html_format(int* n, char* page_to_send, int num_format, const char* string,...){
    char* html = (char*)malloc(2049*sizeof(char));

    char * buffer = my_readfile(page_to_send);

    if (num_format == 1){
        if (buffer)
        {
          // start to process your data / extract strings here...
          *n = sprintf(html, buffer, string);
        }
    }else if(num_format == 2){
        va_list format;
        va_start(format, string);
        if (buffer)
        {
            char* format2 = va_arg(format, char*);
          *n = sprintf(html, buffer, string, format2);
        }
    }
    return html;
}

bool send_html_format(char* page_to_send,
    int sockfd, int num_format, const char* string,...){
    int len_html;
    char* html = prepare_html_format(&len_html, page_to_send, num_format, string);
    char buff[2049];
    int n = sprintf(buff, HTTP_200_FORMAT, len_html);
    // send the header first
    if(!mysendfile(sockfd, buff, n)){
        return false;
    }
    // send the body then
    if(!mysendfile(sockfd, html, len_html)){
        return false;
    }
    return true;
}

bool send_html(char* page_to_send, int sockfd){
    char* html = my_readfile(page_to_send);
    char buff[2049];
    int len_html = strlen(html);
    int n = sprintf(buff, HTTP_200_FORMAT, len_html);
    // send the header first
    if(!mysendfile(sockfd, buff, n)){
        return false;
    }
    // send the body then
    if(!mysendfile(sockfd, html, len_html)){
        return false;
    }
    return true;
}

bool send_html_with_cookie(char* page_to_send, int sockfd,
    int cookie_id){
    char* html = my_readfile(page_to_send);
    char buff[2049];
    int len_html = strlen(html);
    int n = sprintf(buff, HTTP_200_FORMAT_WITH_COOKIE, len_html, cookie_id);
    // send the header first
    if(!mysendfile(sockfd, buff, n)){
        return false;
    }
    // send the body then
    if(!mysendfile(sockfd, html, len_html)){
        return false;
    }
    return true;
}

bool send_fav_icon(char* page_to_send, int sockfd){
    // get the size of the file
    struct stat st;
    stat(page_to_send, &st);
    char buff[2049];
    int n = sprintf(buff, HTTP_200_FORMAT, st.st_size);
    send_body(sockfd, buff, n, page_to_send);
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

bool send_404(int sockfd){
    if (write(sockfd, HTTP_404, HTTP_404_LENGTH) < 0)
    {
        perror("write");
        return false;
    }
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

char* get_img_src(int round){
    char* image_src = (char*)malloc(MAX_PATH_LENGTH*sizeof(char));
    if (round == 1){
        strcpy(image_src, TURN_1_IMAGE_PATH);
    }else{
        strcpy(image_src, TURN_2_IMAGE_PATH);
    }
    return image_src;
}
