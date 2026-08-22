#include <stdio.h>

int RollDice(int n) {
  int o = 7 - n;
  return o;
}

int main() {
  int n = 0;
  printf("Enter a number(1-6): ");
  scanf("%d", &n);
  printf("%d", RollDice(n));
  return 0;
}
