#include <stdio.h>

int factorial(int n)
{
    // Calculating factorial of number
    if (n == 0)
        return 1;
    return n * factorial(n - 1);
}

int main()
{
    int num = 5;
    printf("%d\n", factorial(num));
    return 0;
}
