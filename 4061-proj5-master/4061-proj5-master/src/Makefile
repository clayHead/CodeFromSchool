CC=gcc
CFLAGS=-std=c99 -pthread
DBFLAGS=-g

all: linked_list.o linked_list.h
	$(CC) $(CFLAGS) $(DBFLAGS) -o linked_list.o linked_list.c

clean:
	rm -rf tlist linked_list.o
