/*
  Reading a serial ASCII-encoded string.
 
 This sketch demonstrates the Serial parseInt() function.
 It looks for an ASCII string of comma-separated values.
 It parses them into ints, and uses those to fade an RGB LED.
 
 Circuit: Common-anode RGB LED wired like so:
 * Red cathode: digital pin 3
 * Green cathode: digital pin 5
 * blue cathode: digital pin 6
 * anode: +5V
 
 created 13 Apr 2012
 by Tom Igoe
 
 This example code is in the public domain.
 */

// pins for the LEDs:
const int redPin = 3;
const int greenPin = 5;
const int bluePin = 6;
char serbuf[15];
float wow;
char command;

void disp_vars() {
  Serial.println(redPin);
  Serial.println(greenPin);
  Serial.println(bluePin);
}

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // make the pins outputs:
  pinMode(redPin, OUTPUT); 
  pinMode(greenPin, OUTPUT); 
  pinMode(bluePin, OUTPUT); 
  Serial.setTimeout(100000);
}

void loop() {
  Serial.flush();
  switch (Serial.read()) {
    case '?':
        Serial.println("help");
      break;
    case 'r':
        Serial.println("read");
      break;
    case 'd':
        disp_vars();
      break;
  }
}








