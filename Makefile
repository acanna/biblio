CC = g++
CFLAGS = -c -std=c++11 -Wall -Wextra -Wpedantic
CFLAGS_TEST = -std=c++11 -Wall -Wextra -Wpedantic -pthread -isystem $(GTEST_DIR)/include
LDFLAGS = -lcurl -lpoppler-cpp

SRC = src
BIN = bin

TEST_BIN = bin
GTEST_DIR = test

TESTS = DBLP_unittest
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
TESTS_EXE = $(addprefix $(TEST_BIN)/, $(TESTS))

FILES_FOR_TESTS = DBLPManager.cpp ArticleInfo.cpp jsoncpp.cpp tools.cpp Parser.cpp find_info.cpp Biblio_exception.cpp
FILES = $(FILES_FOR_TESTS) main.cpp 

SOURCES = $(addprefix $(SRC)/, $(FILES))
OBJECTS = $(addprefix $(BIN)/, $(FILES:.cpp=.o))

SOURCES_FOR_TESTS = $(addprefix $(SRC)/, $(FILES_FOR_TESTS))
OBJECTS_FOR_TESTS = $(addprefix $(BIN)/, $(FILES_FOR_TESTS:.cpp=.o))

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
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) $< -o $@

test: $(TESTS_EXE)

clean_tests :
	rm -f $(TESTS_EXE) $(TEST_BIN)/gtest.a $(TEST_BIN)/gtest_main.a $(TEST_BIN)/*.o

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

###########################################################################################
# Test build
###########################################################################################

$(TEST_BIN)/DBLP_unittest.o : $(GTEST_DIR)/DBLP_unittest.cpp $(SOURCES_FOR_TESTS) $(GTEST_HEADERS)
	$(CC) $(CFLAGS_TEST) -c $(GTEST_DIR)/DBLP_unittest.cpp -o $@

$(TEST_BIN)/DBLP_unittest : $(TEST_BIN)/DBLP_unittest.o $(OBJECTS_FOR_TESTS) $(TEST_BIN)/gtest_main.a
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
