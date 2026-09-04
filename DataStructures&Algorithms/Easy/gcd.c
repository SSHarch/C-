#include <stdio.h>

int Getgcd(int a, int b){
    int gcd;
    for(int i = 1; i < a; i++){
        if(a % i == 0 && b % i == 0){
            a / i;
            b / i;
            gcd *= i;
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