CC = clang
CFLAFS = -c -Wall
CGUMBOLIB = -I/usr/local/include -L/usr/local/lib -lgumbo
CCURLLIB = -lcurl
CSQLITELIB = -lsqlite3
CPTHREAD = -lpthread

.PHONY: all, clean

all: generator

generator: generator.c get_rooms.c get_rooms_estate.c get_web.c sqldb.c
	$(CC) $^ $(CFLAGS) $(CGUMBOLIB) $(CCURLLIB) $(CSQLITELIB) $(CPTHREAD) -o $@

clean:
	rm -rf *.o *.gch
		  
