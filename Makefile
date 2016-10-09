CC = g++
CFLAGS = -c -std=c++11 -Wall -Wextra -Wpedantic
LDFLAGS = -lcurl -lpoppler-cpp

SRC = src
BIN = bin

FILES = main.cpp DBLPManager.cpp ArticleInfo.cpp jsoncpp.cpp parser.cpp tools.cpp

SOURCES = $(addprefix $(SRC)/, $(FILES))
OBJECTS = $(addprefix $(BIN)/, $(FILES:.cpp=.o))

EXECUTABLE = $(BIN)/main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

parser.o: tools.o

$(BIN)/%.o: $(SRC)/%.cpp $(SRC)/%.h
	$(CC) $(CFLAGS) $< -o $@

test:

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
