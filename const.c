#include <stdio.h>

int main(int argc, char* argv[]){
  int const a = 5;
  int static const b = 5;
  printf("int const a = %d\n", a);
  printf("%d\n", a++);

  printf("int static const b = %d\n", b);
  printf("%d\n", b++);


  return 0;
}
