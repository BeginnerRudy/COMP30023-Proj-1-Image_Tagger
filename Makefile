CC = gcc
CFLAGS = -Wall

EXE = image_tagger
OBJ = main.o handle_http_request.o http_server.o

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

handle_http_request.o: handle_http_request.c handle_http_request.h html_pages.h cookie.h
	$(CC) $(CFLAGS) -c handle_http_request.c

main.o: main.c http_request.o html_pages.h
	$(CC) $(CFLAGS) -c main.c

http_server.o: http_server.c handle_http_request.h html_pages.h cookie.h
	$(CC) $(CFLAGS) -c http_server.c

clean:
	rm -f $(OBJ) $(EXE)
