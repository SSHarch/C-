#include <stdio.h>

int main() {
  int n = 13;
  int m = 4;
  int c = m;
  int k;
  for (int i = 1; i <= n; i++) {
    if (i % m == 0 && c > n - i) {
      c = n - i;
      k = i;
    }
  }
  printf("%d", k);
  return 0;
}
