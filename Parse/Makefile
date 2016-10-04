CC = gcc
CFLAGS = -c -Wall
CGUMBOLIB = -I/usr/local/include -L/usr/local/lib -lgumbo

.PHONY: all, clean

all: parse_with_tegs

parse_with_tegs: main.o 
		$(CC) main.o $(CGUMBOLIB) -o $@

main.o: parse_with_tegs.c
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm -rf *.o *.gch
