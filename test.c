#include <stdio.h>

void move_pt(char** curr, int n){
    *curr += n;
    printf("move_pt(curr, %d) is %s\n",n,*curr);
}

int main(){
    char* string = "01234567879";
    char** curr  = &string;
    printf("%s\n", *curr);
    int i = 3;
    move_pt(curr, i);
    printf("move_pt(curr, %d) is %s\n",i,*curr);
    return 0;
}
