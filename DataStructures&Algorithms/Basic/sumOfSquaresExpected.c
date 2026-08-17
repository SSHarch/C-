#include <stdio.h>

int main() {
  int n;
  printf("Enter a number: ");
  scanf("%d", &n);
  int k = (n * (n + 1) * (2 * n + 1)) / 6;
  printf("%d\n", k);
  return 0;
}
