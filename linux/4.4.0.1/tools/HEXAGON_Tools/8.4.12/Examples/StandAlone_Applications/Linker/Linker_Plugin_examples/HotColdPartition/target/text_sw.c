#include <stdio.h>

int sw_func1(int a) { return printf("%s %s\n", __FILE__, __FUNCTION__); }

__attribute__((section(".text.sw_func2.cold"))) int sw_func2(int a) {
  return printf("%s %s\n", __FILE__, __FUNCTION__);
}

int sw_func3(int a) { return printf("%s %s\n", __FILE__, __FUNCTION__); }

__attribute__((section(".text.sw_func4.cold"))) int sw_func4(int a) {
  return printf("%s %s\n", __FILE__, __FUNCTION__);
}
