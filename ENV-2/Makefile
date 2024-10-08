all: program

program: main.o Complex.o Sort.o
	g++ -o program main.o Complex.o Sort.o

main.o: main.cpp Complex.h Sort.h
	g++ -c main.cpp -o main.o

Complex.o: Complex.cpp Complex.h
	g++ -c Complex.cpp -o Complex.o

Sort.o: Sort.cpp Sort.h
	g++ -c Sort.cpp -o Sort.o

clean:
	rm -f main.o Complex.o Sort.o program
