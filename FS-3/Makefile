CC = g++
CFLAGS = -Wall -g

all: hacker-rm

hacker-rm: hacker-rm.o
 $(CC) $(CFLAGS) -o hacker-rm hacker-rm.o

hacker-rm.o: hacker-rm.cpp
 $(CC) $(CFLAGS) -c hacker-rm.cpp

clean:
 rm -f hacker-rm hacker-rm.o
