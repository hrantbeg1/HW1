CC = g++
CFLAGS = -Wall -pthread

all: server client

server: server.cpp
	$(CC) $(CFLAGS) server.cpp -o server

client: client.cpp
	$(CC) $(CFLAGS) client.cpp -o client

clean:
	rm -f server client
