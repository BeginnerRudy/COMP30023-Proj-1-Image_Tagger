BIN_DIR = bin
CC = gcc
CFLAGS = -std=c99 -O3 -Wall -Wpedantic

all: mkbin normal 1.1.3 1.1.4 1.2

normal: mkbin client server

1.1.3: mkbin client-1.1.3 server-1.1.3

1.1.4: mkbin client-1.1.4 server-1.1.4

1.2: mkbin client-1.2 server-1.2

%: %.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $<

.PHONY: clean mkbin

clean:
	rm -rf $(BIN_DIR)

mkbin:
	mkdir -p bin
