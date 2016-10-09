CC = g++
CFLAGS = -c -std=c++11 -Wall -Wextra -Wpedantic
LDFLAGS = -lcurl -lpoppler-cpp

SRC = src
BIN = bin

FILES = DBLPManager.cpp ArticleInfo.cpp jsoncpp.cpp tools.cpp Parser.cpp main.cpp 

SOURCES = $(addprefix $(SRC)/, $(FILES))
OBJECTS = $(addprefix $(BIN)/, $(FILES:.cpp=.o))

EXECUTABLE = $(BIN)/main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

$(BIN)/Parser.o: $(BIN)/tools.o

$(BIN)/jsoncpp.o: $(SRC)/jsoncpp.cpp
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/main.o: $(SRC)/main.cpp
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/%.o: $(SRC)/%.cpp $(SRC)/%.h
	$(CC) $(CFLAGS) $< -o $@

test:

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
