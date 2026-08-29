#include <stdio.h>

// Function to find nth term
int term(int n)
{
    return n * (n + 1) / 2;
}


int main()
{
    int n = 4;
    printf("%d", term(n));
    return 0;
}
