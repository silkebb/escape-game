// adapted from: https://srituhobby.com/how-to-make-a-secure-arduino-based-lock-with-keypad-and-lcd-display/

#include <Servo.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Password.h>

Servo servo;
LiquidCrystal lcd(A0, A1, 10, 11, 12, 13);

// FOR RESETTING CODES
// String newPasswordString; //hold the new password
// char newPassword[6]; //charater string of newPasswordString
byte a = 5;
bool doorOpen = false;

// String newBoxNrString; //hold the new box nr
// char newBoxNr[6]; //charater string of box nr
// //byte b = 5;
// //bool valueBoxNr = true;

Password password = Password("4786"); //Enter your password
Password boxNr = Password("333");

byte maxPasswordLength = 6;
byte currentPasswordLength = 0;

byte maxBoxNrLength = 6;
byte currentBoxNrLength = 0;

String boxes[] = {"330", "331","332","333"};
String guessedBoxNr = "";
bool boxExists = false;

//keypad
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
  {'D', 'C', 'B', 'A'},
  {'#', '9', '6', '3'},
  {'0', '8', '5', '2'},
  {'*', '7', '4', '1'},
};
byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

//LEDs
const byte redLED = A3;
const byte greenLED = A4;

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//bool boxNr = false;
 
// phase 0 = start screen --> need to press '#'
// phase 1 = enter box number
// phase 2 = enter password
// repeat phases 1&2 if not entered correctly 
int phase = 0;

void setup() {
  //Serial.begin(9600);
  //use analog pins as digital pins (possible on Arduino Uno)
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  servo.attach(A2);
  servo.write(170);
  // start screen
  lcd.begin(16,2);
  // check that LEDs are working
  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, HIGH);
  delay(200);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
}

void loop() {

  // phase 0: start the process with the '#' key
  if (phase == 0){
    
    lcd.setCursor(0, 0);
    lcd.print("WILLKOMMEN!");
    lcd.setCursor(0, 1);
    lcd.print("BEGINNE MIT #");
    char key = keypad.getKey();
    if (key != NO_KEY) {
      delay(60);
      if (key == '#') {
        lcd.clear();
        blinkGreenLED();
        phase = 1;
      }
    } 
  }
  // phase 1: if process started, type in box nr first
  else if (phase==1) {
    lcd.setCursor(1, 0);
    lcd.print("WAEHLE BOX-NR:");
    char key = keypad.getKey();
    if (key != NO_KEY) {
      delay(60);
      //clear input
      if (key == 'C') {
        resetBoxNr();
      } 
      // "done" - evaluate input
      else if (key == 'D') {
        lcd.clear();
        checkBoxNr();
      }
      else {
          processNumberKey(key);
      }
    }
  }
  // phase 2 
  else if (phase == 2) {
    lcd.setCursor(0, 0);
    lcd.print("PASSWORT:");

    char key = keypad.getKey();
    if (key != NO_KEY) {
      delay(60);
      //'clear' input
      if (key == 'C') {
        resetPassword();
      } 
      //'done' - evaluate input
      else if (key == 'D') {
        if (doorOpen == false) {
          checkPassword();
        }
      } 
      else {
        processNumberKey(key);
      }
    }
  } 
  // phase 3 - let players choose how to proceed
  // choose new box or try other password for current box
  else if (phase ==3) {
    lcd.setCursor(0, 0);
    lcd.print("ANDERE BOX? -> A");
    lcd.setCursor(0, 1);
    lcd.print("ANDERES PW? -> B");
    char choice = keypad.getKey();
    if (choice != NO_KEY) {
      delay(60);
      if (choice == 'A') {
        resetBoxNr();
        resetPassword();
        lcd.clear();
        phase = 1;
      } else if (choice == 'B') {
        resetPassword();
        lcd.clear();
        phase = 2;
      }
    } 
  }
  // phase 4 - door is open, waiting to be closed
  else {
    lcd.setCursor(0, 0);
    lcd.print("ZUM SCHLIESSEN");
    lcd.setCursor(0, 1);
    lcd.print("DRUECKE *");

    char key = keypad.getKey();
    // lock the door again
    if (key == '*') {
      if (doorOpen == true) {
        lcd.clear();
        lockDoor();
        doorOpen = false;
      } 
    }
  }
}

// check if the entered box number exists (doesn't have to be the correct box yet)
void checkBoxNr() {
  for (int n=1; n<=4; n++) {
    if (guessedBoxNr==boxes[n]) {
      boxExists = true;
      phase = 2;
    } 
  }
  if(boxExists==false) {
    digitalWrite(redLED, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("BOX-NR");
    lcd.setCursor(8,0);
    lcd.print(guessedBoxNr);
    lcd.setCursor(0, 1);
    lcd.print("NICHT GEFUNDEN");
    delay(2000);
    lcd.clear();
    resetBoxNr();
    resetLEDs();
  }
  a = 5;
}

void processNumberKey(char key) {
  lcd.setCursor(a, 1);
  lcd.print("*");
  a++;
  if (a == 11) {
    a = 5;
  }
  if (phase == 1) {
    currentBoxNrLength++;
    boxNr.append(key);
    guessedBoxNr+=key;

    if (currentBoxNrLength == maxBoxNrLength) {
      lcd.clear();
      checkBoxNr();
    }
  } else {
    currentPasswordLength++;
    password.append(key);

    if (currentPasswordLength == maxPasswordLength) {
      checkPassword();
    }
  }
}

void checkPassword() {
  resetLEDs();
  if (password.evaluate() && boxNr.evaluate()) {
    digitalWrite(greenLED, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("KORREKT");
    lcd.setCursor(0, 1);
    lcd.print("TUER OEFFNEN...");
    openDoor();
  } else {
    digitalWrite(redLED, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PASSWORT PASST");
    lcd.setCursor(0, 1);
    lcd.print("NICHT ZU BOX");
    delay(2000);
    lcd.clear();
    resetLEDs();
    // phase for the user to choose how to proceed
    // choose new box or try another password for current box
    phase = 3; 
  }
  a = 5;
}

void openDoor() {
  doorOpen = true;
  servo.write(10);
  delay(2000);
  phase = 4;
  resetPassword();
  resetBoxNr();
  lcd.clear();
}

//resets guessed password
void resetPassword() {
  password.reset();
  currentPasswordLength = 0;
  a = 5;
  lcd.clear();
}

//resets guessed box-nr
void resetBoxNr() {
  boxNr.reset();
  guessedBoxNr="";
  currentBoxNrLength = 0;
  a = 5;
  lcd.clear();
}

//void changePassword() {
//  newPasswordString = "1234";
//  newPasswordString.toCharArray(newPassword, newPasswordString.length() + 1); //convert string to char array
//  password.set(newPassword);
//  resetPassword();
//  lcd.clear();
//  lcd.setCursor(0, 0);
//  lcd.print("Password changed");
//  delay(1000);
//  lcd.clear();
//}

void lockDoor() {
  servo.write(170);
  resetAll();
  phase = 0;
  // resetLEDs();
  // if (password.evaluate() && boxNr.evaluate()) {
  //   digitalWrite(greenLED, HIGH);
  //   servo.write(110);
  //   delay(100);
  //   lcd.setCursor(0, 0);
  //   lcd.print("CORRECT PASSWORD");
  //   lcd.setCursor(2, 1);
  //   lcd.print("DOOR LOCKED");
  //   delay(2000);
  //   lcd.clear();
  //   resetBoxNr();
  // } else {
  //   digitalWrite(redLED, HIGH);
  //   lcd.clear();
  //   lcd.setCursor(0, 0);
  //   lcd.print("PASSWORT PASST");
  //   lcd.setCursor(0, 1);
  //   lcd.print("NICHT ZU BOX");
  //   delay(2000);
  //   lcd.clear();
  // }
  // a = 5;
  // resetLEDs();
  // resetPassword();
}

void resetLEDs() {
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
}

void blinkGreenLED() {
  digitalWrite(greenLED, HIGH);
  delay(100);
  digitalWrite(greenLED, LOW);
  delay(50);
  digitalWrite(greenLED, HIGH);
  delay(100);
  digitalWrite(greenLED, LOW);
}

void resetAll(){
  a=5;
  boxExists = false;
  resetLEDs();
  resetPassword();
  resetBoxNr();
}