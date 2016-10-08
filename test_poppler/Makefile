CC = g++
CFLAGS = -c -std=c++11 -Wall -Wextra -Wpedantic
LDFLAGS = -lpoppler-cpp

all: main

main: main.o parser.o tools.o
	$(CC) main.o parser.o tools.o -o main $(LDFLAGS)
	
main.o: main.cpp parser.cpp tools.cpp
	$(CC) $(CFLAGS) main.cpp
	
parser.o: parser.cpp tools.cpp parser.h
	$(CC) $(CFLAGS) parser.cpp
	
tools.o: tools.cpp tools.h
	$(CC) $(CFLAGS) tools.cpp
clean: 
	rm -rf *.o main
