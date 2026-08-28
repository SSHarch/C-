#include <stdio.h>

// Function to find the nth term of series
int term(int n)
{      
    // Loop to add numbers
    int ans = 0;
    for (int i = 1; i <= n; i++)    
        ans += i; 
    
    return ans;
}


int main()
{
    int n = 4;
    printf("%d", term(n));
    return 0;
}
