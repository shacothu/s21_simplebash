#include "module.h"

int parser(int argc, char *argv[], flags *flag) {
  const struct option long_options[] = {
      {"number", no_argument, NULL, 'n'},
      {"number-nonblank", no_argument, NULL, 'b'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {0, 0, 0, 0}};
  int opt = 0;
  while ((opt = getopt_long(argc, argv, "beEnstTv?", long_options, NULL)) !=
         -1) {
    switch (opt) {
      case 'b':
        flag->b = 1;
        break;
      case 'e':
        flag->e = flag->v = 1;
        break;
      case 'E':
        flag->e = 1;
        break;
      case 'n':
        flag->n = 1;
        break;
      case 's':
        flag->s = 1;
        break;
      case 't':
        flag->t = flag->v = 1;
        break;
      case 'T':
        flag->t = 1;
        break;
      case 'v':
        flag->v = 1;
        break;
      case '?':
        flag->error = 1;
        break;
    }
  }
  return optind;  // индекс следующего обрабатываемого элемента argv
}

void process_bflag(char ch, char prev, int *current_line, flags flag) {
  if (flag.b == 1) {
    if (prev == '\n' && ch != '\n') {
      printf("%*d\t", 6, *current_line);
      (*current_line)++;
    }
  }
}

void process_nflag(char prev, int *current_line, flags flag) {
  if (flag.n == 1 && flag.b == 0 && prev == '\n') {
    printf("%*d\t", 6, *current_line);
    (*current_line)++;
  }
}

void process_eflag(char ch, flags flag, char prev) {
  if (flag.e == 1 && ch == '\n') {
    if (flag.b && prev == '\n') {
      // printf("%6c\t$",' ');
      printf("      \t$");
    } else
      printf("$");
  }
}

void process_tflag(char *ch, flags flag) {
  if (flag.t == 1 && *ch == '\t') {
    printf("^");
    *ch = 'I';
  }
}

void process_vflag(char *ch, flags flag) {
  if (flag.v == 1) {
    if ((*ch >= 0 && *ch <= 31) && *ch != '\t' && *ch != '\n') {
      printf("^");
      *ch = *ch + 64;
    } else if (*ch == 127) {
      printf("^");
      *ch = *ch - 64;
    }
  }
}

void readFile(char *path, flags flag) {
  FILE *filename = fopen(path, "r");
  if (filename == NULL) {
    fprintf(stderr, "No such file or directory: %s\n", path);
    return;
  }

  char ch, prev;
  int current_line = 1;
  int squeeze = 0;

  for (prev = '\n'; (ch = getc(filename)) != EOF; prev = ch) {
    if (flag.s == 1) {
      if (ch == '\n' && prev == '\n') {
        if (squeeze == 1) {
          continue;
        }
        squeeze++;
      } else {
        squeeze = 0;
      }
    }

    process_bflag(ch, prev, &current_line, flag);
    process_nflag(prev, &current_line, flag);
    process_eflag(ch, flag, prev);
    process_tflag(&ch, flag);
    process_vflag(&ch, flag);
    putchar(ch);
  }

  fclose(filename);
}