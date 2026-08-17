#include <stdio.h>

int main() {
  int n;
  int k = 0;
  printf("Enter a number: ");
  scanf("%d", &n);
  //  for (int i = 1; i <= n; i++) {
  //  k += i;
  // }
  k = n * (n + 1) / 2;
  printf("%d\n", k);
  return 0;
}
