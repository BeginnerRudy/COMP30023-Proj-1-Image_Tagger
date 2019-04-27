CC = gcc
CFLAGS = -Wall

EXE = image_tagger
OBJ = main.o handle_http_request.o http_server.o player.o response.o game.o


$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)


response.o: response.c 
	$(CC) $(CFLAGS) -c response.c

player.o: player.c 
	$(CC) $(CFLAGS) -c player.c 
game.o: game.c  
	$(CC) $(CFLAGS) -c game.c

handle_http_request.o: handle_http_request.c
	$(CC) $(CFLAGS) -c handle_http_request.c

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c

http_server.o: http_server.c
	$(CC) $(CFLAGS) -c http_server.c


clean:
	rm -f $(OBJ) $(EXE)
