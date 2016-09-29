CC = gcc
CFLAGS = -c -Wall


.PHONY: all, clean

all: generate

generate: main.o sha2.o
		$(CC) main.o sha2.o -o $@

main.o: gen.c
		$(CC) $(CFLAGS) $< -o $@

sha2.o: sha256.c sha256.h
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm -rf *.o *.gch
