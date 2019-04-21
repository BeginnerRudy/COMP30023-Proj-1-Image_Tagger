#include <stdio.h>

int main(int argc, char** argv){
    int i = 6;

    if (i < 4 ){
        if (i == 1){
            printf("1\n");
        }else if (i == 2){
            printf("2\n");
        }else if (i == 3){
            printf("3\n");
        }
    }else  if (i > 3){
        if (i == 4){
            printf("4\n");
        }else if (i == 5){
            printf("5\n");
        }else if (i == 6){
            printf("6\n");
        }
    }else{
        printf("HAHA\n");
    }

    return 0;
}
