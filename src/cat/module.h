#ifndef MODULE_H
#define MODULE_H

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int b, n, s, e, t, v, error;
} flags;

int parser(int argc, char *argv[], flags *flag);
void process_bflag(char ch, char prev, int *current_line, flags flag);
void process_nflag(char prev, int *current_line, flags flag);
void process_eflag(char ch, flags flag, char prev);
void process_tflag(char *ch, flags flag);
void process_vflag(char *ch, flags flag);
void readFile(char *path, flags flag);

#endif  // MODULE_H