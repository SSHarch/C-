#include <bits/stdc++.h>
using namespace std;

// Find the day of the week for the given date
string getDayOfWeek(vector<int> &date)
{
    int d = date[0];
    int m = date[1];
    int y = date[2];

    // Month codes used by the formula
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

    // January and February are treated as months of previous year
    y -= (m < 3);

    // Calculate day number
    int day = (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;

    vector<string> weekDays = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

    return weekDays[day];
}

int main()
{
    vector<int> date = {30, 8, 2010};

    cout << getDayOfWeek(date);

    return 0;
}
