#include <Servo.h>
#include <CapacitiveSensor.h>


// pins for segment display (1 digit)
const int a=2,
          b=3,
          c=4,
          d=5,
          e=6,
          f=7,
          g=8;

// touch sensors
CapacitiveSensor buttonOnOff = CapacitiveSensor(12,11); //pin 11 is sensor = button 
CapacitiveSensor buttonPlus = CapacitiveSensor(A3,10); //pin 10 is sensor = button 
CapacitiveSensor buttonMinus = CapacitiveSensor(A5,9); //pin 9 is sensor = button 

// buzzer pin
const int piezo = A0;

// servos
Servo servo1;
int servo1Pos;
Servo servo2;
int servo2Pos;

// sensor value variables
long valOnOff; 
long valPlus;
long valMinus;
// threshold for button press recognition
const int threshold = 700; 

bool active;

// datatype returned by millis() 
unsigned long startTime; 
unsigned long duration;

// static --> is defined once but keeps last value in next loop, no reset to initial value
static int dispNumber = 0;
bool isOn;
static bool holdingOnOff = false; 

// riddle for number code "recipe"
unsigned long cookingTime;
unsigned long cookingDuration; 
bool riddleHasStarted = false;

void setup()
{
  Serial.begin(9600);
  
  // segment display
  for (int i=2; i<9; i++) {
  	pinMode(i, OUTPUT);
  }
 
  // piezo
  pinMode(piezo, OUTPUT);  
  tone(piezo, 440, 10); // play tone to test if it is working

  // servo
  servo1.attach(A1);
  servo1Pos = 90;
  servo2.attach(A2);
  servo2Pos = 90;
  servo1.write(servo1Pos);
  servo2.write(servo2Pos);

  // sensors
  // pinMode(A1, OUTPUT);  
  // pinMode(A3, OUTPUT);  
  // pinMode(A5, OUTPUT);  
  // pinMode(A2, INPUT);  
  // pinMode(A4, INPUT);  
  // pinMode(A6, INPUT);  


  digitalWrite(a,1);
  digitalWrite(b,1);
  digitalWrite(c,1);
  digitalWrite(d,1);
  digitalWrite(e,1);
  digitalWrite(f,1);
  digitalWrite(g,0);

  delay(100);

  isOn = false;
}


void numToDisp(int x) {

  switch(x) {
  
    case 0:
    zero();
    break;
    
    case 1:
    one();
    break;
    
    case 2:
    two();
    break;
    
    case 3:
    three();
    break;
    
    case 4:
    four();
    break;
    
    case 5:
    five();
    break;
    
    case 6:
    six();
    break;
    
    case 7:
    seven();
    break;
    
    case 8:
    eight();
    break;
    
    case 9:
    nine();
    break;

  }
}

// common cathode vs common anode display --> would have to use 0 as HIGH and 1 as LOW otherwise
void off() {

  digitalWrite(a,0);
  digitalWrite(b,0);
  digitalWrite(c,0);
  digitalWrite(d,0);
  digitalWrite(e,0);
  digitalWrite(f,0);
  digitalWrite(g,0);

}

void zero() {
  
  digitalWrite(a,1);
  digitalWrite(b,1);
  digitalWrite(c,1);
  digitalWrite(d,1);
  digitalWrite(e,1);
  digitalWrite(f,1);
  digitalWrite(g,0);
}

void one() {
  
  digitalWrite(a,0);
  digitalWrite(b,1);
  digitalWrite(c,1);
  digitalWrite(d,0);
  digitalWrite(e,0);
  digitalWrite(f,0);
  digitalWrite(g,0);
}

void two() {
  
  digitalWrite(a,1);
  digitalWrite(b,1);
  digitalWrite(c,0);
  digitalWrite(d,1);
  digitalWrite(e,1);
  digitalWrite(f,0);
  digitalWrite(g,1);

}

void three() {
  
  digitalWrite(a,1);
  digitalWrite(b,1);
  digitalWrite(c,1);
  digitalWrite(d,1);
  digitalWrite(e,0);
  digitalWrite(f,0);
  digitalWrite(g,1);
}

void four() {
  
  digitalWrite(a,0);
  digitalWrite(b,1);
  digitalWrite(c,1);
  digitalWrite(d,0);
  digitalWrite(e,0);
  digitalWrite(f,1);
  digitalWrite(g,1);
 
}

void five() {
  
  digitalWrite(a,1);
  digitalWrite(b,0);
  digitalWrite(c,1);
  digitalWrite(d,1);
  digitalWrite(e,0);
  digitalWrite(f,1);
  digitalWrite(g,1);
}

void six() {
  
  digitalWrite(a,1);
  digitalWrite(b,0);
  digitalWrite(c,1);
  digitalWrite(d,1);
  digitalWrite(e,1);
  digitalWrite(f,1);
  digitalWrite(g,1);
 
}

void seven() {
  
  digitalWrite(a,1);
  digitalWrite(b,1);
  digitalWrite(c,1);
  digitalWrite(d,0);
  digitalWrite(e,0);
  digitalWrite(f,0);
  digitalWrite(g,0);
 
 servo2Pos = 180;

}

void eight() {
  
  digitalWrite(a,1);
  digitalWrite(b,1);
  digitalWrite(c,1);
  digitalWrite(d,1);
  digitalWrite(e,1);
  digitalWrite(f,1);
  digitalWrite(g,1);
 
}

void nine() {
  
  digitalWrite(a,1);
  digitalWrite(b,1);
  digitalWrite(c,1);
  digitalWrite(d,1);
  digitalWrite(e,0);
  digitalWrite(f,1);
  digitalWrite(g,1);

}


void loop() {

  // check sensor values
  valOnOff =  buttonOnOff.capacitiveSensor(30); 
  valPlus = buttonPlus.capacitiveSensor(30);
  valMinus = buttonMinus.capacitiveSensor(30);

  Serial.print(valOnOff);
  Serial.print("\t");
  Serial.print(valPlus);
  Serial.print("\t");
  Serial.println(valMinus);
  
  // start holding the on/off button
  if (valOnOff > threshold && holdingOnOff == false) {
    startTime = millis();
    holdingOnOff = true; 
    Serial.println("START holding");
  }
  // releasing the on/off button
  else if (valOnOff < threshold && holdingOnOff == true) {
    holdingOnOff = false;
    Serial.println("END holding");
  }
    
  if (holdingOnOff == true) {
    duration = millis() - startTime;
    Serial.println("holding on");

    // long hold
    if (duration >= 800) {
      // einschalten
      if(isOn == false) {
        Serial.println("TURN ON");
      	tone(piezo, 494, 10); // play tone (B4 = 494 Hz)
        delay(500);
      }
      // ausschalten
      else {
        Serial.println("TURN OFF");
        tone(piezo, 440, 10); // play tone (A4 = 440 Hz)
        delay(500);
      }
      isOn = !isOn;
      duration = 0;
    }
  }
  
  if (isOn == true) {
    // minus-button gedrückt & nicht weniger als 0 anzeigen
    if (valMinus > threshold && dispNumber !=0) {
      Serial.println("---");
      dispNumber = dispNumber-1;
      delay(300);
    }

    // plus-button gedrückt & nicht mehr als 9 anzeigen
    if (valPlus > threshold && dispNumber !=9) {
      Serial.println("+++");

      // wenn von 8 auf 9 hinaufgeschaltet wird, starte code-rätsel
      if (dispNumber == 8) {
        cookingTime = millis();
      }

      dispNumber = dispNumber+1;
      delay(300);
    }

    numToDisp(dispNumber);

    // "rezept": mind 10 sek auf stärkster hitze kochen, dann auf mittlere hitze zurückschalten
    if (dispNumber == 9) {
      servo1Pos = 0;
      servo1.write(servo1Pos);
      cookingDuration = millis()-cookingTime;

      if (cookingDuration > 10000) {
        riddleHasStarted = true;
      }
    }

    if (riddleHasStarted && dispNumber == 5) {
      servo2Pos = 180;
      servo2.write(servo2Pos);
    }

  } 
  else {
  
  	off(); 
    dispNumber = 0;
    
  }

  delay(100);
}