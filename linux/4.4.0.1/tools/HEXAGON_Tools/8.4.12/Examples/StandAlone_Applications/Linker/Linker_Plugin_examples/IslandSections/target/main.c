#include <stdio.h>

#include "island.h"

int main(int argc, char *argv[]) {
  int result = 0;

  printf("\n\n");

  result += f1(argc);
  result += f2(argc);
  result += f3(argc);
  result += f4(argc);

  printf("\nResult = %d\n", result);
  return result;
}
