all: server client1 client2

client1: client1.c
	gcc -o client1 client1.c

client2: client2.c
	gcc -o client2 client2.c

server: server.c
	gcc -o server server.c

clean:
	rm -f *.o client1 client2 server
