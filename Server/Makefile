CC = g++
CFLAGS = -Wall -pthread
TARGETS = server client

all: $(TARGETS)

server: server.cpp
	$(CC) $(CFLAGS) server.cpp -o server

client: client.cpp
	$(CC) $(CFLAGS) client.cpp -o client

clean:
	rm -f $(TARGETS)

.PHONY: all clean
