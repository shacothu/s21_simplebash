SHELL := /bin/bash
CC=gcc
CFLAGS=-Wall -Werror -Wextra
FILE=test1.txt
TARGET=s21_grep
PATTERN=hello
SRCS=grep.c module.c
HEADERS=module.h

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) 

rebuild:clean $(TARGET)

test1: $(TARGET)
	sh run_tests.sh
clang:
	cp -f ../../materials/linters/.clang-format ../../src/grep
	clang-format -n *.c
	clang-format -n *.h

clean:
	rm -f $(TARGET)

.PHONY: all rebuild clean test
 
