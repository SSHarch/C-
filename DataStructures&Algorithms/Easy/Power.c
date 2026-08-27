#include <stdio.h>
#include <stdbool.h>

bool isPower(int x, int y)
{
    // The only power of 1 is 1 itself
    if (x == 1)
        return (y == 1);

    // Repeatedly compute power of x
    int pow = 1;
    while (pow < y)
        pow *= x;

    // Check if power of x becomes y
    return (pow == y);
}

int main()
{
    printf("%s\n", isPower(10, 1) ? "true" : "false");
    printf("%s\n", isPower(1, 20) ? "true" : "false");
    printf("%s\n", isPower(2, 128) ? "true" : "false");
    printf("%s\n", isPower(2, 30) ? "true" : "false");
    return 0;
}
