CXX = g++

CXXFLAGS = -Wall -std=c++17

TARGET = main_file

OBJS = main.o Complex.o Sort.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp Complex.h Sort.h
	$(CXX) $(CXXFLAGS) -c main.cpp

Complex.o: Complex.cpp Complex.h
	$(CXX) $(CXXFLAGS) -c Complex.cpp

Sort.o: Sort.cpp Sort.h
	$(CXX) $(CXXFLAGS) -c Sort.cpp

clean:
	rm -f $(OBJS) $(TARGET)
