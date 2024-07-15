#include "module.h"

void pattern_add(flags *flag, char *pattern) {
  if (flag->len_pattern != 0) {
    strcat(flag->pattern + flag->len_pattern, "|");
    flag->len_pattern++;
  }
  strcat(flag->pattern + flag->len_pattern, pattern);
  flag->len_pattern += strlen(pattern);
}

void add_file(flags *flag, char *path) {
  FILE *filename = fopen(path, "r");
  if (filename == NULL) {
    if (!flag->s) perror(path);
    exit(1);
  }
  char line[1024];
  while (fgets(line, sizeof(line), filename) != NULL) {
    char *newline = strchr(line, '\n');
    if (newline) *newline = '\0';
    pattern_add(flag, line);
  }
  fclose(filename);
}

flags parser(int argc, char *argv[]) {
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
  if (flag.len_pattern == 0 && !flag.f) {
    pattern_add(&flag, argv[optind]);
    optind++;
  }
  if (argc - optind == 1) {
    flag.h = 1;
  }
  return flag;
}

void output_line(char *line, int n) {
  for (int i = 0; i < n; i++) {
    putchar(line[i]);
  }
  if (line[n - 1] != '\n') putchar('\n');
}

void print_match(regex_t *re, char *line) {
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

int grep(flags *flag, const char *path, regex_t *reg) {
  FILE *file = fopen(path, "r");
  if (!file) {
    if (!flag->s) perror(path);
    exit(1);
  }
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int line_count = 0;
  int match_count = 0;
  int line_matched = 0;
  while ((read = getline(&line, &len, file)) != -1) {
    line_count++;
    int match = (regexec(reg, line, 0, NULL, 0) == 0);
    if ((match && !flag->v) || (!match && flag->v)) {
      match_count++;
      line_matched = 1;
      if (!flag->c && !flag->l) {
        if (!flag->h) printf("%s:", path);
        if (flag->n) printf("%d:", line_count);
        if (flag->o && !flag->v) {
          print_match(reg, line);
        } else {
          output_line(line, read);
        }
      }
    }
  }
  free(line);
  fclose(file);
  if (flag->c && !flag->l) {
    if (!flag->h) printf("%s:", path);
    printf("%d\n", match_count);
  }
  if (flag->l && line_matched) {
    printf("%s\n", path);
  }
  return line_matched;
}

void output(flags flag, int argc, char *argv[]) {
  regex_t re = {0};
  int error = regcomp(&re, flag.pattern, REG_EXTENDED | flag.i);
  if (error) perror("ERROR");

  int total_files_matched = 0;

  for (int i = optind; i < argc; i++) {
    int matched = grep(&flag, argv[i], &re);
    if (flag.l && matched) {
      total_files_matched++;
    }
  }

  if (flag.l && flag.c) {
    printf("%d\n", total_files_matched);
  }

  regfree(&re);
}