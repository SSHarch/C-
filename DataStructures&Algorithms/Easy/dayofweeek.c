#include <iostream>
#include <vector>
using namespace std;

bool isLeapYear(int year) {
    return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

string getDayOfWeek(vector<int>& date) {
    int day = date[0];
    int month = date[1];
    int year = date[2];

    vector<int> daysInMonth = {
        31,28,31,30,31,30,
        31,31,30,31,30,31
    };

    vector<string> weekDays = {
        "Monday","Tuesday","Wednesday",
        "Thursday","Friday","Saturday","Sunday"
    };

    long long totalDays = 0;

    // Count complete years before current year
    for (int y = 1; y < year; y++) {
        totalDays += isLeapYear(y) ? 366 : 365;
    }

    // Count complete months before current month
    for (int m = 1; m < month; m++) {
        if (m == 2 && isLeapYear(year))
            totalDays += 29;
        else
            totalDays += daysInMonth[m - 1];
    }

    // Add days in current month
    totalDays += day - 1;

    // 01-01-0001 was Monday
    return weekDays[totalDays % 7];
}

int main() {
    vector<int> date = {17, 4, 1435};

    cout << getDayOfWeek(date) << endl;

    return 0;
}
