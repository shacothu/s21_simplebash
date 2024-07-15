#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct flags {
  int e, i, v, c, l, n, h, s, f, o;
  char pattern[1024];
  int len_pattern;
} flags;

void pattern_add(flags* flag, char* pattern) {
  int n = strlen(pattern);
  if (flag->len_pattern != 0) {
    strcat(flag->pattern + flag->len_pattern, "|");
    flag->len_pattern++;
  }

  flag->len_pattern +=
      sprintf(flag->pattern + flag->len_pattern, "(%s)", pattern);
}

void add_file(flags* flag, char* path) {
  FILE* filename;
  filename = fopen(path, "r");
  if (filename == NULL) {
    if (!flag->s) perror(path);

    exit(1);
  }
  char* line = NULL;
  size_t memlen = 0;
  int read = getline(&line, &memlen, filename);
  while (read != -1) {
    if (line[read - 1] == '\n') line[read - 1] = '\0';
    pattern_add(flag, line);
    read = getline(&line, &memlen, filename);
  }
  free(line);
  fclose(filename);
}

flags parser(int argc, char* argv[]) {
  flags flag = {0};
  int opt = 0;
  while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (opt) {
      case 'e':
        flag.e = 1;
        pattern_add(&flag, optarg);
        break;
      case 'i':
        flag.i = REG_ICASE;
        break;
      case 'v':
        flag.v = 1;
        break;
      case 'c':
        flag.c = 1;
        break;
      case 'l':
        flag.c = 1;
        flag.l = 1;
        break;
      case 'n':
        flag.n = 1;
        break;
      case 'h':
        flag.h = 1;
        break;
      case 's':
        flag.s = 1;
        break;
      case 'f':
        flag.f = 1;
        add_file(&flag, optarg);
        break;
      case 'o':
        flag.o = 1;
        break;
    }
  }
  if (flag.len_pattern == 0) {
    pattern_add(&flag, argv[optind]);
    optind++;
  }
  if (argc - optind == 1) {
    flag.h = 1;
  }
  return flag;
}

void output_line(char* line, int n) {
  for (int i = 0; i < n; i++) {
    putchar(line[i]);
  }
  if (line[n - 1] != '\n') putchar('\n');
}

void print_all_matches(regex_t* re, char* line) {
  regmatch_t match;
  int offset = 0;
  while (1) {
    int result = regexec(re, line + offset, 1, &match, 0);
    if (result != 0) {
      break;
    }
    for (int i = match.rm_so; i < match.rm_eo; i++) {
      putchar(line[i]);
    }
    putchar('\n');
    offset += match.rm_eo;
  }
}

void grep(flags* flag, char* path, regex_t* reg) {
  FILE* filename;
  filename = fopen(path, "r");
  if (filename == NULL) {
    if (!flag->s) perror(path);
    perror(path);
    exit(1);
  }
  char* line = NULL;
  size_t memlen = 0;
  int read = 0;
  int line_count = 1;
  int c = 0;
  read = getline(&line, &memlen, filename);

  while (read != -1) {
    int result = regexec(reg, line, 0, NULL, 0);
    if ((result == 0 && !flag->v) || (flag->v && result != 0)) {
      if (!flag->c && !flag->l) {
        if (!flag->h) {
          printf("%s:", path);
        }
        if (flag->n) {
          printf("%d:", line_count);
        }
        if (flag->o) {
          print_all_matches(reg, line);
        } else {
          output_line(line, read);
        }
      }
      c++;
    }
    read = getline(&line, &memlen, filename);
    line_count++;
  }
  free(line);
  if (flag->c && !flag->l) {
    if (!flag->h) printf("%s:", path);
    printf("%d\n", c);
  }
  if (flag->l && c > 0) printf("%s\n", path);
  fclose(filename);
}

void output(flags flag, int argc, char* argv[]) {
  regex_t re = {0};
  int error = regcomp(&re, flag.pattern, REG_EXTENDED | flag.i);
  if (error) perror("ERROR");
  for (int i = optind; i < argc; i++) {
    grep(&flag, argv[i], &re);
  }
}

int main(int argc, char* argv[]) {
  flags flag = parser(argc, argv);
  output(flag, argc, argv);
  return 0;
}