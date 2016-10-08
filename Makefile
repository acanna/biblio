CC=g++
CFLAGS=-c -std=c++11 -Wall -Wextra -Wpedantic
LDFLAGS= -lcurl -lpoppler-cpp

SRC=src
BIN=bin

FILES=main.cpp DBLPManager.cpp ArticleInfo.cpp jsoncpp.cpp

SOURCES=$(addprefix $(SRC)/, $(FILES))
OBJECTS=$(addprefix $(BIN)/, $(FILES:.cpp=.o))

EXECUTABLE=$(BIN)/main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

$(BIN)/%.o: $(SRC)/%.cpp 
	$(CC) $(CFLAGS) $< -o $@

test: 

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)