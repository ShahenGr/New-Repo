CC = gcc
CFLAFS = -c -Wall
CGUMBOLIB = -I/usr/local/include -L/usr/local/lib -lgumbo
CCURLLIB = -lcurl
CSQLITELIB = -lsqlite3

.PHONY: all, clean

all: get_rooms

get_rooms: get_rooms_main.c get_rooms.c sqldb.c
	$(CC) $^ $(CFLAGS) $(CGUMBOLIB) $(CCURLLIB) $(CSQLITELIB)  -o $@


clean:
	rm -rf *.o *.gch
		  
