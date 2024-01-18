#include <iostream>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cmath>

void round_minutes(std::tm* instance) {
    // Calculate total seconds of the hour
    int seconds_of_hour = instance->tm_min * 60 + instance->tm_sec;

    // Divisors to round to
    std::vector<int> divisors = {10*60, 15*60};

    // Find the divisor closest to the current seconds of the hour
    int closest_divisor = *std::min_element(divisors.begin(), divisors.end(), [seconds_of_hour](int x, int y) {
        return std::abs(x - seconds_of_hour % x) < std::abs(y - seconds_of_hour % y);
    });

    // Calculate the diff of the current second to the closest divisor
    int remainder = seconds_of_hour % closest_divisor;

    // If the remainder is greater than or equal to half the divisor, round up, otherwise round down
    int roundedSecond;
    if (remainder >= closest_divisor / 2) {
        roundedSecond = seconds_of_hour - remainder + closest_divisor;
    } else {
        roundedSecond = seconds_of_hour - remainder;
    }

    int minutes = roundedSecond / 60;

    // If rounding up brought us to 60 minutes, we need to roll over to the next hour
    if (minutes == 60) {
        instance->tm_hour += 1;
        instance->tm_min = 0;
        instance->tm_sec = 0;
    } else {
        instance->tm_min = minutes;
        instance->tm_sec = 0;
    }
}

int get_rounded_hour_12h_format(int hour, int minutes) {
    // If minutes are at least 30, we need to roll over to the next hour
    if (minutes >= 30) {
        hour += 1;
    }

    if (hour == 0) {
        return 12;
    } else if (hour > 12) {
        return hour - 12;
    } else {
        return hour;
    }
}

void tell() {
    std::time_t t = std::time(0);   // get time now
    std::tm* now = std::localtime(&t);

    std::cout << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << std::endl;

    round_minutes(now);
    int hours = get_rounded_hour_12h_format(now->tm_hour, now->tm_min);
    int minutes = now->tm_min;

    std::cout << hours << " - " << minutes << std::endl;
}

int main() {
    tell();
    return 0;
}