#include <stdio.h>
#include "http_server.h"
#include "player.h"

int main(int argc, char **argv){
  // run the http web game server
  http_server(argc, argv);
  return 0;
}
