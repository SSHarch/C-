#include <math.h>
#include <stdio.h>

int main() {
  int n;
  printf("Enter a number: ");
  scanf("%d", &n);
  int k = 0;
  for (int i = 0; i <= n; i++) {
    k += pow(i, 2);
  }
  printf("%d\n", k);
  return 0;
}
