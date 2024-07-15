#include "module.h"

int main(int argc, char* argv[]) {
  flags flag = parser(argc, argv);
  output(flag, argc, argv);
  return 0;
}
// -i