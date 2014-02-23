
/* 
 Stepper Motor Control - one revolution
 
 This program drives a unipolar or bipolar stepper motor. 
 The motor is attached to digital pins 8 - 11 of the Arduino.
 
 The motor should revolve one revolution in one direction, then
 one revolution in the other direction.  
 
  
 Created 11 Mar. 2007
 Modified 30 Nov. 2009
 by Tom Igoe
 
 */

#include <Stepper.h>

const int stepsPerRevolution = 2000;  // change this to fit the number of steps per revolution
                                     // for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 7,8,9,10);            

void setup() {
  pinMode(6, OUTPUT);      // sets the digital pin as output
  digitalWrite(6, HIGH);   // sets the LED on
  pinMode(11, OUTPUT);      // sets the digital pin as output
  digitalWrite(11, HIGH);   // sets the LED on
  // set the speed at 60 rpm:
  myStepper.setSpeed(30);
  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {
  // step one revolution  in one direction:
   Serial.println("clockwise");
  myStepper.step(3000);
  delay(500);
  
   // step one revolution in the other direction:
  Serial.println("counterclockwise");
  myStepper.step(-3000);
  delay(500); 
}

