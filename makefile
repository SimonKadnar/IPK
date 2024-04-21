CC=gcc

CFLAGS=-std=gnu99 -Wall -Wextra -pedantic -lm

all: hinfosvc

generator: hinfosvc.o
	$(CC) $(CFLAGS) $^ -o hinfosvc

generator.o:hinfosvc.c
	$(CC) $(CFLAGS) -c -o hinfosvc.o hinfosvc.c

clean: hinfosvc.o hinfosvc
	rm -rf $^
