#include <stdio.h>

int bench = 405;
int bw; // Bodyweight
char name[100];

int main() {
  printf(
      "And if your dream still lives, if that castle still gleams just as\n "
      "brightly in your eyes - then it is your obligation to lay the stones\n "
      "that surround you now. Let us begin the chant of offering. \n");

  printf("%d\n", bench);
  printf("Kg...\n");

  fputs("What is your bodyweight: ", stdout);
  scanf("%d", &bw);

  printf("Your bodyweight is: %d\n", bw);

  printf("Enter a string: ");

  return 0;
}
