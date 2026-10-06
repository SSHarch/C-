#include <stdio.h>

bool Divideby11(int n){
    if(n % 11 == 0){
        return true;
    }
    else{
        return false;
    }
}

int main(){
    printf("Enter a number: ");
    char c = '0';
    scanf("%s", &c);
    printf("%s", Divideby11(c) ? "true" : "false");
    return 0;
}