#define numberOfSeconds(_time_) ((_time_ / 1000) % 60)
#define numberOfMinutes(_time_) (((_time_ / 1000) / 60) % 60)

#include "SevSeg.h"
#include "pitches_h.h"
// melodies from https://www.hackster.io/dennis-v/buzz-wire-deluxe-650cbc

SevSeg sevseg;

//time limit in milliseconds (40 minutes = 1000x60x40)
unsigned long timeLimit = 2400000;
bool gameStart = false; // changes value once all badges are lifted off / placed on the LDR sensors
bool finished = false;
const int buzzerPin = 22;
const int ldrPin1 = A0;
const int ldrPin2 = A1;
const int ldrPin3 = A2;
const int ldrPin4 = A3;
const int ldrPin5 = A4;
int ldrValue1 = 0;
int ldrValue2 = 0;
int ldrValue3 = 0;
int ldrValue4 = 0;
int ldrValue5 = 0;
unsigned long stopTime = timeLimit;

void setup() {
  pinMode(buzzerPin,OUTPUT);

 // SevSeg library
  byte numDigits = 4;
  byte digitPins[] = {10, 11, 12, 13};
  byte segmentPins[] = {2,3,4,5,6,7,8,9};
  
  bool resistorsOnSegments = false;
  byte hardwareConfig = COMMON_CATHODE;
  bool updateWithDelays = false;
  bool leadingZeros = true;
  bool disableDecPoint = false;
  
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);

  Serial.begin(9600);
}


  // play the level start melody
void melodyLevelStart() {
  tone(buzzerPin, NOTE_C4, 200);
  delay(600);
  tone(buzzerPin, NOTE_C4, 200);
  delay(600);
  tone(buzzerPin, NOTE_C4, 200);
  delay(600);
  tone(buzzerPin, NOTE_C5);
  delay(600);
  noTone(buzzerPin);
}

// play the level finished melody
void melodyLevelFinished() {
  tone(buzzerPin, NOTE_C4);
  delay(300);
  tone(buzzerPin, NOTE_D4);
  delay(300);
  tone(buzzerPin, NOTE_E4);
  delay(300);
  tone(buzzerPin, NOTE_F4);
  delay(300);
  tone(buzzerPin, NOTE_G4);
  delay(300);
  noTone(buzzerPin);
}

// play the wire touched melody
void melodyLevelWireTouched() {
  tone(buzzerPin, NOTE_A4);
  delay(300);
  tone(buzzerPin, NOTE_GS4);
  delay(300);
  tone(buzzerPin, NOTE_G4);
  delay(1000);
  noTone(buzzerPin);
}

// play the game over melody
void melodyLevelGameOver() {
  tone(buzzerPin, NOTE_A4);
  delay(300);
  tone(buzzerPin, NOTE_GS4);
  delay(300);
  tone(buzzerPin, NOTE_G4);
  delay(300);
  tone(buzzerPin, NOTE_FS4);
  delay(300);
  tone(buzzerPin, NOTE_F4);
  delay(300);
  tone(buzzerPin, NOTE_E4);
  delay(300);
  tone(buzzerPin, NOTE_DS4);
  delay(300);
  tone(buzzerPin, NOTE_D4);
  delay(300);
  tone(buzzerPin, NOTE_CS4);
  delay(300);
  tone(buzzerPin, NOTE_C4);
  delay(1000);
  noTone(buzzerPin);
}

// play the champion melody
void melodyChampion() {
  tone(buzzerPin, NOTE_F4);
  delay(300);
  tone(buzzerPin, NOTE_A4);
  delay(300);
  tone(buzzerPin, NOTE_C5);
  delay(200);
  noTone(buzzerPin);  
  delay(100);
  tone(buzzerPin, NOTE_A4);
  delay(100);
  tone(buzzerPin, NOTE_C5);
  delay(300);
  noTone(buzzerPin);
}

void loop() {

  if (gameStart == false) {

    // if game has not started yet
    sevseg.setNumber(4000,2);
    sevseg.refreshDisplay();

    // check sensor data
    ldrValue1 = analogRead(ldrPin1);
    ldrValue2 = analogRead(ldrPin2);
    ldrValue3 = analogRead(ldrPin3);
    ldrValue4 = analogRead(ldrPin4);
    ldrValue5 = analogRead(ldrPin5);
  
    // if all sensor values above threshold --> set gameStart to true
    if (ldrValue1>50 && ldrValue2>50 && ldrValue3>50 && ldrValue4>50 && ldrValue5>50) {
      melodyLevelStart();
      gameStart = true;
      // buzzer noise: tone(pinNr, frequency, duration)
      //tone(buzzerPin, 500, 100);
    } 

  }

  // if game has started
  if (gameStart == true) {
    // Calculate time remaining
    unsigned long startTime = millis();
    unsigned long timeRemaining = timeLimit - millis() + startTime;
    

    while(timeRemaining > 0 && finished==false) {
      // for displaying in mm.ss format
      int seconds = numberOfSeconds(timeRemaining);
      int minutes = numberOfMinutes(timeRemaining);
      int time = minutes*100 + seconds;
      sevseg.setNumber(time,2);
      sevseg.refreshDisplay();

      // update time remaining
      timeRemaining = timeLimit - millis() + startTime;

      // check sensor data
      ldrValue1 = analogRead(ldrPin1);
      ldrValue2 = analogRead(ldrPin2);
      ldrValue3 = analogRead(ldrPin3);
      ldrValue4 = analogRead(ldrPin4);
      ldrValue5 = analogRead(ldrPin5);
      
      if (ldrValue1<50 && ldrValue2<50 && ldrValue3<50 && ldrValue4<50 && ldrValue5<50) {
        // buzzer noise: tone(pinNr, frequency, duration)
        stopTime = time;
        finished = true;
        melodyChampion();
        break;
      }
    }

    if(finished==true) {
      sevseg.setNumber(stopTime,2);
      sevseg.refreshDisplay();
    }

  }
  

}
