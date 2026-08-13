#include <stdio.h>

int PRs[3][3] = {{10, 20, 35}, {30, 40, 65}};

int main() {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 3; j++) {
      printf("%d", PRs[i][j]);
    }
    printf("\n");
  }
  return 0;
}
