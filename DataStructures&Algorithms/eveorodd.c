#include <math.h>
#include <stdbool.h>
#include <stdio.h>

bool isEven(int n) {
  if ((n & 1) == 0)
    return true;
  else
    return false;
}

int main() {
  int n;
  printf("Enter a number: ");
  scanf("%d", &n);
  if (isEven(n)) {
    printf("true\n");
  } else {
    printf("false\n");
  }
  return 0;
}
