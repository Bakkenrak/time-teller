#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <cmath>
#include <time.h>

struct Time {
    int hour;
    int min;
    int sec;
};

// WIFI credentials
const char *ssid     = "MY_WIFI_SID";
const char *password = "my-wifi-pw";

// Init NTP Client 
const long utcOffsetInSeconds = 3600;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// init audio player
SoftwareSerial mySoftwareSerial(13, 15); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
const int audioPlayerBusyPin = 12;

const int volumeButtonPin = 4;
int initialVolumeButtonState;
int volumeLevel = 20; //Set volume value. From 0 to 30

const byte sensorTriggerPin = 14;
const byte sensorEchoPin = 16;
const long sensorCheckIntervalMillis = 1000;
// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long lastSensorCheckMillis = 0;

void setup()
{
  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  connectToWifi();

  configTime("CET-1CEST,M3.5.0/02,M10.5.0/03", "pool.ntp.org");
  
  connectAudioPlayer();
  
  pinMode(sensorTriggerPin, OUTPUT);
  pinMode(sensorEchoPin, INPUT);

  pinMode(volumeButtonPin, INPUT);
  initialVolumeButtonState = digitalRead(volumeButtonPin);
}

void connectToWifi() {
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println();
  Serial.println(F("WIFI connected"));
}

void connectAudioPlayer() {
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial, true, true)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  // configure audio player's busy pin as input
  pinMode(audioPlayerBusyPin, INPUT);
}

void loop()
{
  if (digitalRead(volumeButtonPin) != initialVolumeButtonState) {
    toggleVolume();
  }

  unsigned long currentMillis = millis();
  bool shouldCheckSensor = currentMillis - lastSensorCheckMillis >= sensorCheckIntervalMillis;
  if (shouldCheckSensor) {
    lastSensorCheckMillis = currentMillis;
    if (getSensorDistance() < 30) {
      tellTime();
      waitForReady();
    }
  }
}

int getSensorDistance() {
  digitalWrite(sensorTriggerPin, LOW);
  delayMicroseconds(2);

  digitalWrite(sensorTriggerPin, HIGH);
  delayMicroseconds(10);

  long duration = pulseIn(sensorEchoPin, HIGH);
  int distance = duration/58;

  if (distance > 300 || distance <= 0){
    Serial.println("Out of sensor range!");
  } else {
    Serial.println("Distance to object: " + String(distance) + " cm");
  }
  return distance;
}

void waitForReady() {
  delay(1000); // initial wait to not skip over loading the currently playing file
  while(!isReadyToPlay()) {
    delay(50);
  }
}

bool isReadyToPlay() {
  return digitalRead(audioPlayerBusyPin) == HIGH;
}

Time getRoundedTime(int hour, int min, int sec) {
    // Calculate total seconds of the hour
    int secondsOfHour = min * 60 + sec;

    // Divisors to round to
    int tenMinSecs = 10*60;
    int fifteenMinSecs = 15*60;

    // Find the divisor closest to the current seconds of the hour
    int diffClosestBy10 = std::min(tenMinSecs - secondsOfHour % tenMinSecs, secondsOfHour % tenMinSecs);
    int diffClosestBy15 = std::min(fifteenMinSecs - secondsOfHour % fifteenMinSecs, secondsOfHour % fifteenMinSecs);
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

    Time roundedTime;
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
  Time currentTime = getCurrentTime();
  Time roundedTime = getRoundedTime(currentTime.hour, currentTime.min, currentTime.sec);
  int hours = getRoundedHour12hFormat(roundedTime.hour, roundedTime.min);
  int minutes = roundedTime.min;

  Serial.print("Spelled out as ");
  Serial.print(hours);
  Serial.print(" - ");
  Serial.println(minutes);
  Serial.println();

  playSound(hours, minutes);
}

void toggleVolume() {
  volumeLevel -= 2;
  if (volumeLevel <= 0) {
    volumeLevel = 30;
  }
  Serial.print("Toggling volume to ");
  Serial.println(volumeLevel);

  playSound(0, 0);
}

void playSound(int folderNr, int fileNr) {
  applyVolumeLevel();
  myDFPlayer.playFolder(folderNr, fileNr);
}

void applyVolumeLevel() {
  myDFPlayer.volume(volumeLevel);
}

Time getCurrentTime() {
  time_t now;                       // this are the seconds since Epoch (1970) - UTC
  time(&now);                       // read the current time
  tm tm;
  localtime_r(&now, &tm);           // update the structure tm with the current time
  Serial.print(tm.tm_hour);         // hours since midnight  0-23
  Serial.print(":");
  Serial.print(tm.tm_min);          // minutes after the hour  0-59
  Serial.print(":");
  Serial.println(tm.tm_sec);          // seconds after the minute  0-61*

  Time currentTime;
  currentTime.hour = tm.tm_hour;
  currentTime.min = tm.tm_min;
  currentTime.sec = tm.tm_sec;
  return currentTime;
}
