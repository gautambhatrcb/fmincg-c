CC = g++
CFLAGS= -c -Wall
LDFLAGS=

example: fmincg.o example.o
	$(CC) $(LDFLAGS) fmincg.o example.o -o example

fmincg.o: fmincg.c fmincg.h
	$(CC) $(CFLAGS) fmincg.c
example.o: example.c
	$(CC) $(CFLAGS) example.c

all: example
	
run: example
	./example
	
clean:
	rm -rf *.o example