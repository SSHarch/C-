#include <iostream>
using namespace std;

int gcd(int a, int b)
{
    // Everything divides 0
    if(a==0 || b==0) return max(a, b);
    
    // Find Minimum of a and b
    int result = min(a, b);
    while (result > 0) {
        if (a % result == 0 && b % result == 0) {
            break;
        }
        result--;
    }

    // Return gcd of a and b
    return result;
}

int main()
{
    int a = 20, b = 28;
    cout << gcd(a, b);
    return 0;
}
