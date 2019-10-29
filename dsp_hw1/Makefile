.PHONY: all clean run
CC=g++
#CFLAGS=-c
LDFLAGS=-lm
TARGET=train test

all: $(TARGET)

train: src/train.cpp
	$(CC) -o $@ $^  $(LDFLAGS) -Iinc

test: src/test.cpp
	$(CC) -o $@ $^  $(LDFLAGS) -Iinc

test2: src/test_hmm.c
	$(CC) -o $@ $^  $(LDFLAGS) -Iinc

clean:
	rm -f $(TARGET)
