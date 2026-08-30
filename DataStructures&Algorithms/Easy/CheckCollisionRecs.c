#include <stdio.h>

struct Point {
    int x, y;
};

// Returns true if two rectangles (l1, r1) and (l2, r2)
// overlap
int doOverlap(struct Point l1, struct Point r1, struct Point l2, struct Point r2)
{
   if (l1.x > r2.x || l2.x > r1.x)
        return 0;

    // If one rectangle is above the other
    if (r1.y > l2.y || r2.y > l1.y)
        return 0;

    return 1;
}

/* Driver program to test above function */
int main()
{
    struct Point l1 = { 0, 10 }, r1 = { 10, 0 };
    struct Point l2 = { 5, 5 }, r2 = { 15, 0 };
    if (doOverlap(l1, r1, l2, r2))
        printf("Rectangles Overlap");
    else
        printf("Rectangles Don't Overlap");
    return 0;
}
