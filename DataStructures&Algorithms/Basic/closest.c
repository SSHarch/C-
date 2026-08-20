#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  int n = -15;
  int m = 6;
  int c = m;
  int k;
  for (int i = 1; i <= n; i++) {
    if (i % m == 0 && abs(c) > abs(n - i)) {
      c = n - i;
      k = i;
    }
  }
  printf("%d", k);
  printf("%d", c);
  return 0;
}
