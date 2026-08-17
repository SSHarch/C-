#include <stdio.h>

int main() {
  int n;
  printf("Enter a number: ");
  scanf("%d", &n);
  for (int i = 1; i < 11; i++) {
    int j = n * i;
    printf("%d * %d = %d\n", n, i, j);
  }
  return 0;
}
