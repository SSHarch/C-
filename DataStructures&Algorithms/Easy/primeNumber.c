#include <stdio.h>
#include <stdbool.h>
#include <math.h>

bool IsPrimeNumber(int n){
    if(n <= 1)
        return false;

    for(int i = 2; i * i <=n; i++)
        if(n % i == 0)
            return false;
    return true;
}

int main(){
    int n = 0;
    printf("Enter a number: ");
    scanf("%d", &n);
    if(IsPrimeNumber(n)) printf("%s", "Prime");
    else printf("%s","not Prime");
       return 0;
}