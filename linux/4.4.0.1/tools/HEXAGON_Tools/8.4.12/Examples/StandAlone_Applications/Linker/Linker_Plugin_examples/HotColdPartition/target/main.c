#include <stdio.h>

int sw_func1(int a);
int sw_func3(int a);
int comp_func1(int a);
int comp_func3(int a);

int main(int argc, char *argv[]) {
  int result = 0;

  printf("\n\n");

  result += sw_func1(argc);
  result += sw_func3(argc);
  result += comp_func1(argc);
  result += comp_func3(argc);

  printf("\nResult = %d\n", result);
  return result;
}
