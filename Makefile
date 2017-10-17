CC=g++
CFLAGS=-c -Wall -g
LDFLAGS=-g -lm

SENDER_MAIN=./src/sendfile.cpp
RECEIVER_MAIN=./src/recvfile.cpp

SOURCES=$(wildcard ./src/*.cpp) $(wildcard ./src/common/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
SENDER_OBJECTS=$(filter-out $(RECEIVER_MAIN:.cpp=.o), $(OBJECTS))
RECEIVER_OBJECTS=$(filter-out $(SENDER_MAIN:.cpp=.o), $(OBJECTS))

SENDER_EXECUTABLE=./sendfile
RECEIVER_EXECUTABLE=./recvfile

.PHONY: all bin clean

all: bin

bin: $(SENDER_EXECUTABLE) $(RECEIVER_EXECUTABLE)

$(SENDER_EXECUTABLE): $(SENDER_OBJECTS)
	$(CC) $(LDFLAGS) $(SENDER_OBJECTS) -o $@

$(RECEIVER_EXECUTABLE): $(RECEIVER_OBJECTS)
	$(CC) $(LDFLAGS) $(RECEIVER_OBJECTS) -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm $(OBJECTS)
	-rm $(SENDER_EXECUTABLE)
	-rm $(RECEIVER_EXECUTABLE)
