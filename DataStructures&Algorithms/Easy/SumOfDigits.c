#include <stdio.h>

int SumOfDigits(int n) {
  int sum = 0;
  while (n != 0) {
    sum += n % 10;
    n /= 10;
  }
  return sum;
}

int main() {
  int n;
  printf("Enter a number: ");
  scanf("%d", &n);
  printf("%d", SumOfDigits(n));
  return 0;
}
