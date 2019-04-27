CC = gcc
CFLAGS = -Wall

EXE = image_tagger
OBJ = main.o handle_http_request.o http_server.o player.o response.o game.o


$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)


response.o: response.c response.h
	$(CC) $(CFLAGS) -c response.c

player.o: player.c player.h
	$(CC) $(CFLAGS) -c player.c player.h
game.o: game.c game.h player.o
	$(CC) $(CFLAGS) -c game.c game.h

handle_http_request.o: handle_http_request.c handle_http_request.h html_pages.h game.o response.o
	$(CC) $(CFLAGS) -c handle_http_request.c

main.o: main.c handle_http_request.o html_pages.h
	$(CC) $(CFLAGS) -c main.c

http_server.o: http_server.c http_server.h
	$(CC) $(CFLAGS) -c http_server.c


clean:
	rm -f $(OBJ) $(EXE)
