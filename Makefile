CC = gcc
CFLAGS = -Wall

EXE = image_tagger
OBJ = main.o http_request.o

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

http_request.o: http_request.c http_request.h html_pages.h
	$(CC) $(CFLAGS) -c http_request.c

main.o: main.c http_request.o html_pages.h
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(OBJ) $(EXE)
