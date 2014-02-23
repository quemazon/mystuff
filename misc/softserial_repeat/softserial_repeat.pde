// include the SoftwareSerial library so you can use its functions:
#include <SoftwareSerial.h>

#define rxPin 8
#define txPin 10
#define ledPin 3

// set up a new serial port
SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);
byte pinState = 0;

void setup()  {
  // define pin modes for tx, rx, led pins:
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  // set the data rate for the SoftwareSerial port
  mySerial.begin(4800);
  delay(500);
  mySerial.println("Hello World - SoftwareSerial");
}

void loop() {
  // listen for new serial coming in:
  char someChar = mySerial.read();
//  mySerial.print("testing, blah, blah");
  // print out the character:
  mySerial.print(someChar);
  // toggle an LED just so you see the thing's alive.  
  // this LED will go on with every OTHER character received:
  toggle(ledPin);
}

void toggle(int pinNum) {
  // set the LED pin using the pinState variable:
  digitalWrite(pinNum, pinState); 
  // if pinState = 0, set it to 1, and vice versa:
  pinState = !pinState;
}

