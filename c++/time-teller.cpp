#include <iostream>
#include <cmath>

struct RoundedTime {
    int hour;
    int min;
    int sec;
};

RoundedTime getRoundedTime(int hour, int min, int sec) {
    // Calculate total seconds of the hour
    int secondsOfHour = min * 60 + sec;

    // Divisors to round to
    int tenMinSecs = 10*60;
    int fifteenMinSecs = 15*60;

    // Find the divisor closest to the current seconds of the hour
    int diffClosestBy10 = std::min(std::abs(tenMinSecs - secondsOfHour % tenMinSecs), secondsOfHour % tenMinSecs);
    int diffClosestBy15 = std::min(std::abs(fifteenMinSecs - secondsOfHour % fifteenMinSecs), secondsOfHour % fifteenMinSecs);
    int closestDivisor;
    if (diffClosestBy10 < diffClosestBy15) {
        closestDivisor = tenMinSecs;
    } else {
        closestDivisor = fifteenMinSecs;
    }

    // Calculate the diff of the current second to the closest divisor
    int remainder = secondsOfHour % closestDivisor;

    // If the remainder is greater than or equal to half the divisor, round up, otherwise round down
    int roundedSecond;
    if (remainder >= closestDivisor / 2) {
        roundedSecond = secondsOfHour - remainder + closestDivisor;
    } else {
        roundedSecond = secondsOfHour - remainder;
    }

    int minutes = roundedSecond / 60;

    RoundedTime roundedTime;
    // If rounding up brought us to 60 minutes, we need to roll over to the next hour
    if (minutes == 60) {
        roundedTime.hour = hour + 1;
        roundedTime.min = 0;
        roundedTime.sec = 0;
    } else {
        roundedTime.hour = hour;
        roundedTime.min = minutes;
        roundedTime.sec = 0;
    }
    return roundedTime;
}

int getRoundedHour12hFormat(int hour, int minutes) {
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

void tell(int hour, int min, int sec) {
    std::cout << hour << ":" << min << ":" << sec << std::endl;

    RoundedTime roundedTime = getRoundedTime(hour, min, sec);
    int hours = getRoundedHour12hFormat(roundedTime.hour, roundedTime.min);
    int minutes = roundedTime.min;

    std::cout << "Rounded to: " << hours << ":" << minutes << std::endl;
}

int main() {
    tell(16, 55, 1);
    return 0;
}