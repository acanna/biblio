all: main

main: main.o parser.o
	g++ main.o parser.o -o main -lpoppler-cpp
main.o: main.cpp
	g++ -c main.cpp
parser.o: parser.cpp
	g++ -c parser.cpp

clean: rm -rf *.o main
