CC = g++

CFLAGS = -c -std=c++11 -pg -Wall -Wextra -Wpedantic
CFLAGS_TEST = -std=c++11 -Wall -Wextra -Wpedantic -pthread -isystem $(GTEST_DIR)/include
LDFLAGS = -pg -lcurl -lpoppler-cpp -ltesseract -llept -lpthread -lconfig++ -lopencv_core -lopencv_highgui -lopencv_imgproc -ltinyxml2 -lsqlite3

SRC = src
BIN = bin

TEST_BIN = bin
GTEST_DIR = lib/googletest-master/googletest
JSON_DIR = lib/json
REQ_DIR = Requesters
TEST_DIR = test

TESTS = bibliotest 

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
TESTS_EXE = $(addprefix $(TEST_BIN)/, $(TESTS))

FILES_REQUESTERS = Requester.cpp DBLPRequester.cpp SpringerRequester.cpp ArxivRequester.cpp NatureRequester.cpp ScopusRequester.cpp ScienceDirectRequester.cpp
FILES_REQUESTERS_SOURCES = $(addprefix $(REQ_DIR)/, $(FILES_REQUESTERS))

FILES_FOR_TESTS = Config.cpp $(FILES_REQUESTERS_SOURCES) ArticleInfo.cpp tools.cpp PictureParser.cpp BiblioManager.cpp Database.cpp 

HEADERS_FOR_TESTS = $(addprefix $(SRC)/, $(FILES_FOR_TESTS:.cpp=.h))

FILES = $(FILES_FOR_TESTS) main.cpp 

SOURCES = $(addprefix $(SRC)/, $(FILES))
OBJECTS = $(addprefix $(BIN)/, $(FILES:.cpp=.o)) $(BIN)/jsoncpp.o

SOURCES_FOR_TESTS = $(addprefix $(SRC)/, $(FILES_FOR_TESTS))
OBJECTS_FOR_TESTS = $(addprefix $(BIN)/, $(FILES_FOR_TESTS:.cpp=.o)) $(BIN)/jsoncpp.o

EXECUTABLE = $(BIN)/main

all: prepare $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

$(BIN)/Parser.o: $(BIN)/tools.o

$(BIN)/jsoncpp.o: $(JSON_DIR)/jsoncpp.cpp
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/main.o: $(SRC)/main.cpp
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/%.o: $(SRC)/%.cpp $(SRC)/%.h $(HEADERS_FOR_TESTS)
	$(CC) $(CFLAGS) $< -o $@

prepare:
	mkdir -p $(BIN)
	mkdir -p $(BIN)/$(REQ_DIR)

test: prepare $(TESTS_EXE)

clean:
	rm -rf $(BIN)

###########################################################################################
# Test build
###########################################################################################

$(TEST_BIN)/bibliotest.o : $(TEST_DIR)/bibliotest.cpp $(SOURCES_FOR_TESTS) $(GTEST_HEADERS) 
	$(CC) $(CFLAGS_TEST) -c $(TEST_DIR)/bibliotest.cpp -o $@

$(TEST_BIN)/bibliotest : $(TEST_BIN)/bibliotest.o $(OBJECTS_FOR_TESTS) $(TEST_BIN)/gtest_main.a
	$(CC) $(CFLAGS_TEST) -lpthread $^ -o $@ $(LDFLAGS) 

###########################################################################################
# Google Test Library
###########################################################################################
# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
$(TEST_BIN)/gtest-all.o : $(GTEST_SRCS_)
	$(CC) $(CFLAGS_TEST) -I$(GTEST_DIR) -c \
            $(GTEST_DIR)/src/gtest-all.cc -o $@

$(TEST_BIN)/gtest_main.o : $(GTEST_SRCS_)
	$(CC) $(CFLAGS_TEST) -I$(GTEST_DIR) -c \
            $(GTEST_DIR)/src/gtest_main.cc -o $@

$(TEST_BIN)/gtest.a : $(TEST_BIN)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^ 

$(TEST_BIN)/gtest_main.a : $(TEST_BIN)/gtest-all.o $(TEST_BIN)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

###########################################################################################
# Test bib
###########################################################################################

tex: $(EXECUTABLE)
	make -C tex clean
	make -C tex all

tex_clean:
		make -C tex clean

.PHONY: tex_clean tex
