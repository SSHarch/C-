#include <stdio.h>

int main() {
  int n;
  printf("Enter a number: ");
  scanf("%d", &n);
  if (n % 2 > 0) {
    printf("false\n");
  } else if (n % 2 <= 0) {
    printf("true\n");
  }
  return 0;
}
