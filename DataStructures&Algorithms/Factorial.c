#include <stdio.h>

int factorial(int n)
{
    
    // Calculating factorial of number
    int ans = 1, i;
    for (i = 2; i <= n; i++)
        ans *= i;
    return ans;
}

int main()
{
    int num = 5;
    printf("%d\n", factorial(num));
    return 0;
}
