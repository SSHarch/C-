#include <stdio.h>

int Getgcd(int a, int b){
    int gcd = 1;
    for(int i = 2; i < a; i++){
        if(a % i == 0 && b % i == 0){
            a /= i;
            b /= i;
            gcd *= i;
            i--;
        }
    }
    return gcd;
}
int main(){
    int a = 36;
    int b = 60;
    printf("%d", Getgcd(a, b));
    return 0;
}