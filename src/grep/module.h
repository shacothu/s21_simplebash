#ifndef S21_GREP_H_
#define S21_GREP_H_

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct flags {
  int e, i, v, c, l, n, h, s, f, o;
  char pattern[1024];
  int len_pattern;
  int mem_pattern;
} flags;

void pattern_add(flags *flag, char *pattern);
void add_file(flags *flag, char *path);
flags parser(int argc, char *argv[]);
void output_line(char *line, int n);
void print_match(regex_t *re, char *line);
int grep(flags *flag, const char *path, regex_t *reg);
void output(flags flag, int argc, char *argv[]);

#endif  // S21_GREP_H_