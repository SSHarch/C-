#include <stdio.h>

int main() {
    /* 
       Declare and initialize a 2×2 integer array.
       arr[0][0] = 10, arr[0][1] = 20,
       arr[1][0] = 30, arr[1][1] = 40 
    */
    int arr[2][2] = { {10, 20}, {30, 40} };

    printf("2D Array Elements:\n");
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }

    return 0;
}
