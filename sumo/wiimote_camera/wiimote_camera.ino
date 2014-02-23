// Wii Remote IR sensor  test sample code  by kako
// modified output for Wii-BlobTrack program by RobotFreak

#include <Wire.h>

int IRsensorAddress = 0xB0;
int slaveAddress;
int ledPin = 13;
boolean ledState = false;
byte data_buf[16];
int i;

int Ix[4];
int Iy[4];
int s;

void Write_2bytes(byte d1, byte d2)
{
    Wire.beginTransmission(slaveAddress);
    Wire.write(d1); Wire.write(d2);
    Wire.endTransmission();
}

void setup()
{
    slaveAddress = IRsensorAddress >> 1;   // This results in 0x21 as the address to pass to TWI
    Serial.begin(38400);
    pinMode(ledPin, OUTPUT);      // Set the LED pin as output
    Wire.begin();
    // IR sensor initialize
    Write_2bytes(0x30,0x01); delay(10); //Control byte, allows modification of settings

    Write_2bytes(0x06,50); delay(10); // 10 MAXSIZE - Maximum blob size. Wii uses values from 0x62 to 0xc8 (98 to 200).
    Write_2bytes(0x08,60); delay(10); // 15 GAIN - Sensor Gain. Smaller values = higher gain. Numerical gain is proportional to 1/2^(n/16) for n<0x40
    Write_2bytes(0x1A,59); delay(10); // 10 GAINLIMIT - Sensor Gain Limit. Must be less than GAIN for camera to function. No other effect?
    Write_2bytes(0x1B,20); delay(10); // * MINSIZE - Minimum blob size. Wii uses values from 3 to 5
    Write_2bytes(0x33,0x33); delay(10); // 
    Write_2bytes(0x30,0x08); delay(10); //Was Out of order, needs to be at end
    delay(100);
	// http://letsmakerobots.com/node/7752 (Source for this entire sketch)
	// http://blog.makezine.com/archive/2008/11/hacking-the-wiimote-ir-ca.html
	// http://wiki.wiimoteproject.com/IR_Sensor
	// Level 1: p0 = 0x72, p1 = 0x20, p2 = 0x1F, p3 = 0x03
	// Level 2: p0 = 0xC8, p1 = 0x36, p2 = 0x35, p3 = 0x03
	// Level 3: p0 = 0xAA, p1 = 0x64, p2 = 0x63, p3 = 0x03
	// Level 4: p0 = 0x96, p1 = 0xB4, p2 = 0xB3, p3 = 0x04
	// Level 5: p0 = 0x96, p1 = 0xFE, p2 = 0xFE, p3 = 0x05
	// p0: MAXSIZE: Maximum blob size. Wii uses values from 0x62 to 0xc8
	// p1: GAIN: Sensor Gain. Smaller values = higher gain
	// p2: GAINLIMIT: Sensor Gain Limit. Must be less than GAIN for camera to function. No other effect?
	// p3: MINSIZE: Minimum blob size. Wii uses values from 3 to 5
}
void loop()
{
    ledState = !ledState;
    if (ledState) { digitalWrite(ledPin,HIGH); } else { digitalWrite(ledPin,LOW); }

    //IR sensor read
    Wire.beginTransmission(slaveAddress);
    Wire.write(0x36);
    Wire.endTransmission();

    Wire.requestFrom(slaveAddress, 16);        // Request the 2 byte heading (MSB comes first)
    for (i=0;i<16;i++) { data_buf[i]=0; }
    i=0;
    while(Wire.available() && i < 16) { 
        data_buf[i] = Wire.read();
        i++;
    }

    Ix[0] = data_buf[1];
    Iy[0] = data_buf[2];
    s   = data_buf[3];
    Ix[0] += (s & 0x30) <<4;
    Iy[0] += (s & 0xC0) <<2;

    Ix[1] = data_buf[4];
    Iy[1] = data_buf[5];
    s   = data_buf[6];
    Ix[1] += (s & 0x30) <<4;
    Iy[1] += (s & 0xC0) <<2;

    Ix[2] = data_buf[7];
    Iy[2] = data_buf[8];
    s   = data_buf[9];
    Ix[2] += (s & 0x30) <<4;
    Iy[2] += (s & 0xC0) <<2;

    Ix[3] = data_buf[10];
    Iy[3] = data_buf[11];
    s   = data_buf[12];
    Ix[3] += (s & 0x30) <<4;
    Iy[3] += (s & 0xC0) <<2;

    for(i=0; i<4; i++)
    {
      if (Ix[i] < 1000)
        Serial.print(" ");
      if (Ix[i] < 100)  
        Serial.print(" ");
      if (Ix[i] < 10)  
        Serial.print(" ");
      Serial.print( int(Ix[i]) );
      Serial.print(",");
      if (Iy[i] < 1000)
        Serial.print(" ");
      if (Iy[i] < 100)  
        Serial.print(" ");
      if (Iy[i] < 10)  
        Serial.print(" ");
      Serial.print( int(Iy[i]) );
      if (i<3)
        Serial.print(",");
    }
    Serial.println("");
    delay(3);
}


