#include <stdio.h>

int ReverseDigits(int n) {
  int Revnum = 0;
  while (n > 0) {
    Revnum = Revnum * 10 + n % 10;
    n = n / 10;
  }
  return Revnum;
}

int main() {
  int n;
  printf("Enter a number: ");
  scanf("%d", &n);
  printf("%d", ReverseDigits(n));
  return 0;
}
