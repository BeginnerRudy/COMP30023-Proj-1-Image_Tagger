CC = gcc
CFLAGS = -Wall

EXE = image_tagger
OBJ = main.o handle_http_request.o http_server.o cookie.o


$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

cookie.o: cookie.c cookie.h
	$(CC) $(CFLAGS) -c cookie.c

handle_http_request.o: handle_http_request.c handle_http_request.h html_pages.h cookie.o
	$(CC) $(CFLAGS) -c handle_http_request.c

main.o: main.c handle_http_request.o html_pages.h
	$(CC) $(CFLAGS) -c main.c

http_server.o: http_server.c http_server.h
	$(CC) $(CFLAGS) -c http_server.c


clean:
	rm -f $(OBJ) $(EXE)
