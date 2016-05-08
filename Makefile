CC = gcc

all:
	$(CC) server.c -o server && $(CC) client.c -o  client

server: server.c
	$(CC) server.c -o server

client: client.c
	$(CC) client.c -o client

.PHONY: clean
clean:
	rm -rf server client