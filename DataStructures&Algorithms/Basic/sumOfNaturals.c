#include <stdio.h>

int main() {
  int n;
  int k = 0;
  printf("Enter a number: ");
  scanf("%d", &n);
  for (int i = 0; i <= n; i++) {
    k += i;
  }
  printf("%d\n", k);
  return 0;
}
