CC=gcc
CFLAGS=-Wall -Wextra -g
TARGET=tarsau

SRC=src/main.c src/archive.c src/extract.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
