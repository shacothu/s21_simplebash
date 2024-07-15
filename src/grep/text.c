#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

typedef struct flags {
  int e, i, v, c, l, n, h, s, f, o;
  char* pattern;
} flags;

// Прототипы функций
int parser(int argc, char *argv[], flags *flag, char* temp[], int* temp_count, char* temp_f[], int* temp_count_f);
char *pattern_find(char *source_line, char *expression, flags flag, regmatch_t *pmatch);
char *pattern_find_file(char *source_line, flags *flag, int temp_count, char **temp, int temp_count_f, char **temp_f, int line_number, char *filename, int files_count, int *match_count);
void output(FILE *file, flags* flag, int temp_count, char **temp, int temp_count_f, char **temp_f, char *filename, int files_count, int *match_count);
void output_line(char *source_line, char *filename, int line_number, int files_count, flags *flag);

int parser(int argc, char *argv[], flags *flag, char* temp[], int* temp_count, char* temp_f[], int* temp_count_f){
  int opt = 0;
  while ((opt = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) != -1) {
    switch (opt) {
      case 'e':
        flag->e = 1;
        temp[*temp_count] = optarg;
        (*temp_count)++;
        break;
      case 'i':
        flag->i = 1;
        break;
      case 'v':
        flag->v = 1;
        break;
      case 'c':
        flag->c = 1;
        break;
      case 'l':
        flag->l = 1;
        break;
      case 'n':
        flag->n = 1;
        break;
      case 'h':
        flag->h = 1;
        break;
      case 's':
        flag->s = 1;
        break;
      case 'f':
        flag->f = 1;
        temp_f[*temp_count_f] = optarg;
        (*temp_count_f)++;
        break;
      case 'o':
        flag->o = 1;
        break;
      case '?':
        perror("usage: cat [-belnstuv] [file ...]");
        optind = -1;
        break;
      default:
        fprintf(stderr, "s21_grep: invalid option - %c\n", opt);
        exit(1);
        break;
    }
  }

  for (int i = 0; i < *temp_count_f; i++) {
    FILE *file = fopen(temp_f[i], "r");
    if (file == NULL) {
      fprintf(stderr, "Error opening file %s\n", temp_f[i]);
      exit(1);
    } else {
      fclose(file);
    }
  }
  
  return optind;
}

char *pattern_find(char *source_line, char *expression, flags flag, regmatch_t *pmatch){
  char *result = NULL;
  if (source_line[0] != 0 && source_line[0] != '\n' || flag.f) {
    regex_t regex_expression;
    int error = 0;
    if (flag.i) {
      error = regcomp(&regex_expression, expression, REG_EXTENDED | REG_ICASE);
    } else {
      error = regcomp(&regex_expression, expression, REG_EXTENDED);
    }

    if (error != 0) {
      return NULL;
    }

    if (regexec(&regex_expression, source_line, 1, pmatch, 0) == 0) {
      result = source_line;
    }
    regfree(&regex_expression);
  }
  return result;
}

char *pattern_find_file(char *source_line, flags *flag, int temp_count, char **temp, int temp_count_f, char **temp_f, int line_number, char *filename, int files_count, int *match_count){
  char *result = NULL;
  size_t length;
  regmatch_t pmatch[1];
  char *line = NULL;

  for (int i = 0; i < temp_count; i++) {
    result = pattern_find(source_line, temp[i], *flag, pmatch);
    if (result != NULL || (result == NULL && flag->v))
      break;
  }

  for (int i = 0; i < temp_count_f; i++) {
    FILE *file = fopen(temp_f[i], "r");
    if (file == NULL) {
      continue;
    }

    while (getline(&line, &length, file) != -1) {
      result = pattern_find(source_line, line, *flag, pmatch);
      if ((result != NULL && !flag->o) || (result != NULL && flag->v))
        break;
    }
    free(line);
    fclose(file);
  }
  return result;
}

void output(FILE *file, flags* flag, int temp_count, char **temp, int temp_count_f, char **temp_f, char *filename, int files_count, int *match_count){
  size_t length = 0;
  int line_number = 1, end_line;
  char *line_file = NULL;
  
  while ((end_line = getline(&line_file, &length, file)) != -1) {
    int check_match = 0;
    if (line_file != NULL) {char *match_part = pattern_find_file(line_file, flag, temp_count, temp, temp_count_f, temp_f, line_number, filename, files_count, match_count);
      if (match_part == NULL)
        check_match = 0;
      else {
        check_match = 1;
      }

      if ((check_match == 1 && flag->v == 0) || (check_match == 0 && flag->v == 1)) {
        (*match_count)++;
        if (flag->c == 0 && flag->l == 0) {
          output_line(line_file, filename, line_number, files_count, flag);
        }
      }
      line_number++;
    }
  }
  
  free(line_file);

  if (flag->c) {
    if (flag->l) {
      if (*match_count > 0)
        *match_count = 1;
      else
        *match_count = 0;
    }
    if (files_count == 1 || flag->h == 1)fprintf(stdout, "%d\n", *match_count);
    else
      fprintf(stdout, "%s:%d\n", filename, *match_count);
  }
  if (flag->l == 1) {
    if (*match_count != 0) {
      fprintf(stdout, "%s\n", filename);
    }
  }
}

void output_line(char *source_line, char *filename, int line_number, int files_count, flags *flag){
  if (files_count == 1 || flag->h == 1) {
    if (flag->n)
      printf("%d:%s", line_number, source_line);
    else
      printf("%s", source_line);
  } else {
    if (flag->n)
      printf("%s:%d:%s", filename, line_number, source_line);
    else
      printf("%s:%s", filename, source_line);
  }

  if (source_line[strlen(source_line) - 1] != '\n') printf("\n");
}

int main(int argc, char *argv[]) {
  flags flag = {0};
  char *files[argc], *temp[argc], *temp_f[argc];
  int files_count = 0, temp_count = 0, temp_count_f = 0, match_count = 0;

  int optind = parser(argc, argv, &flag, temp, &temp_count, temp_f, &temp_count_f);
  
  for (int i = optind; i < argc; i++) {
    files[files_count++] = argv[i];
  }

  if (temp_count == 0 && temp_count_f == 0) {
    fprintf(stderr, "s21_grep: No template\n");
    exit(1);
  } else {
    for (int i = 0; i < files_count; i++) {
      FILE *file = fopen(files[i], "r");
      if (file != NULL) {
        output(file, &flag, temp_count, temp, temp_count_f, temp_f, files[i], files_count, &match_count);
        fclose(file);
        match_count = 0;
      } else {
        if (!flag.s)
          fprintf(stderr, "s21_grep: %s: No such file or directory\n",
files[i]);
      }
    }
  }
  return 0;
}