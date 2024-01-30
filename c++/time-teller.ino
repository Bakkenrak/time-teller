#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <cmath>

const char *ssid     = "MY_WIFI_SSID";
const char *password = "changeme";

const long utcOffsetInSeconds = 3600;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


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

void tellTime() {
  RoundedTime roundedTime = getRoundedTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
  int hours = getRoundedHour12hFormat(roundedTime.hour, roundedTime.min);
  int minutes = roundedTime.min;

  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  Serial.print("Spelled out as ");
  Serial.print(hours);
  Serial.print(":");
  Serial.println(minutes);
  Serial.println();

  // play "es_ist" (file no 1)
  if (minutes > 0) {
    // play minutes (file no 200 + minutes)
  }
  // play hours (file no 100 + hours)
  if (minutes == 0) {
    // play minutes (file no 200)
  }
}

void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
}

void loop() {
  timeClient.update();

  // if trigger signal:
  tellTime();

  delay(10000);
}
