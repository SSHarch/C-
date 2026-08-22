#include <stdio.h>

int SumOfDigits(int n) {
  int i = n / 100;
  int j = (n - i * 100) / 10;
  int k = (n - i * 100 - j * 10) / 1;
  return i + j + k;
}

int main() {
  int n;
  printf("Enter a number: ");
  scanf("%d", &n);
  printf("%d", SumOfDigits(n));
  return 0;
}
