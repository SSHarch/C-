#include <stdio.h>

int nthFibonacci(int n) {
    
    // base case
    if (n <= 1){
        return n;
    }
    // sum of the two preceding 
    // Fibonacci numbers
    return nthFibonacci(n - 1) + nthFibonacci(n - 2);
}

int main(){
    int n = 5;
    int result = nthFibonacci(n);
    printf("%d
", result);
    return 0;
      }
