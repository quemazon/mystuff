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
  mySerial.begin(300);
  mySerial.println("Hello World - SoftwareSerial");
}

void loop() {
  mySerial.println("Hello World - SoftwareSerial");
  delay(5000);
}


