#include <stdio.h>

int comp_func1(int a) { return printf("%s %s\n", __FILE__, __FUNCTION__); }

__attribute__((section(".text.comp_func2.cold"))) int comp_func2_cold(int a) {
  return printf("%s %s\n", __FILE__, __FUNCTION__);
}

int comp_func3(int a) { return printf("%s %s\n", __FILE__, __FUNCTION__); }

__attribute__((section(".text.comp_func4.cold"))) int comp_func4_cold(int a) {
  return printf("%s %s\n", __FILE__, __FUNCTION__);
}
