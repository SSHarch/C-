#include <stdio.h>

// Function to find gcd of a and b
int gcd(int n1, int n2)
{
    if (n1 == 0)
        return n2;
    return gcd(n2 % n1, n1);
}

// Function to add two fractions
void addFraction(int a[], int b[], int result[])
{
    // Finding gcd of den1 and den2
    int den = gcd(a[1], b[1]);

    // Denominator of final fraction obtained
    // finding LCM of den1 and den2
    // LCM * GCD = a * b 
    den = (a[1] * b[1]) / den;

    // Changing the fractions to have same denominator
    // Numerator of the final fraction obtained
    int num = (a[0]) * (den / a[1]) + (b[0]) * (den / b[1]);

    // finding the common factor of numerator and denominator
    int common_factor = gcd(num, den);

    // Converting the result into simpler 
    // fraction by dividing them with common factor 
    den = den / common_factor;
    num = num / common_factor;
    result[0] = num;
    result[1] = den;
}

int main()
{
    int a[] = {1,2};;
    int b[] = {3,2};;
    int ans[2];
    addFraction(a, b, ans);
    printf("%d, %d", ans[0], ans[1]);
    return 0;
}
