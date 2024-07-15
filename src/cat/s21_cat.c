#include "module.h"

int main(int argc, char *argv[]) {
  int number_of_files;
  flags option = {0};
  number_of_files = parser(argc, argv, &option);

  if (option.error == 1) {
    fprintf(stderr, "usage: cat [-belnstuv] [file ...]");
  } else {
    while (number_of_files < argc) {
      char *path;
      path = argv[number_of_files];
      readFile(path, option);
      number_of_files++;
    }
  }
  return 0;
}