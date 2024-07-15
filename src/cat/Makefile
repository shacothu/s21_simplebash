SHELL := /bin/bash
CC = gcc
CFLAGS=-Wall -Wextra -Werror
FILE=cat_test.txt
TARGET=s21_cat

SRCS=s21_cat.c module.c

HEADERS=module.h

all: $(TARGET)
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

rebuild:clean all

clean:
	rm -f $(TARGET)
test1: $(TARGET)
	./test_cat.sh
test: $(TARGET)
	-diff <(./s21_cat $(FILE)) <(cat $(FILE))
	-diff <(./s21_cat -n $(FILE)) <(cat -n $(FILE))
	-diff <(./s21_cat -b $(FILE)) <(cat -b $(FILE))
	-diff <(./s21_cat -s $(FILE)) <(cat -s $(FILE))
	-diff <(./s21_cat -e $(FILE)) <(cat -e $(FILE))
	-diff <(./s21_cat -t $(FILE)) <(cat -t $(FILE))
	-diff <(./s21_cat --squeeze-blank $(FILE)) <(cat -s $(FILE))
	-diff <(./s21_cat --number $(FILE)) <(cat -n $(FILE))
	-diff <(./s21_cat --number-nonblank $(FILE)) <(cat -b $(FILE))
clang:
	cp -f ../../materials/linters/.clang-format ../../src/cat
	clang-format -n *.c
	clang-format -n *.h

.PHONY: all rebuild clean
 
