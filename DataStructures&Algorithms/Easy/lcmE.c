#include <iostream>
using namespace std;

// function for gcd
int gcd(int a, int b) {
    return (b == 0) ? a : gcd(b, a % b);
}

int lcm(int a, int b) {
    return (a / gcd(a, b)) * b;
}

int main() {
    int a = 10, b = 5;
    cout << lcm(a, b); 
    return 0;
}
