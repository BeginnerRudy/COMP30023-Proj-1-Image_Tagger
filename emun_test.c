#include <stdio.h>

typedef enum{
  GET,
  POST,
  UNKNOWN
} METHOD;

int main(int argc, char* argv[]){
  METHOD method = GET;
  printf("%d\n", method);
  return 0;
}
