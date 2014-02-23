// Example of using the PVision library for interaction with the Pixart sensor on a WiiMote
// This work was derived from Kako's excellent Japanese website
// http://www.kako.com/neta/2007-001/2007-001.html

// Steve Hobley 2009 - www.stephenhobley.com

#include <Wire.h>
#include <PVision.h>

PVision ircam;
byte result;

void setup()
{
  Serial.begin(115200);
  ircam.init(75, 1, 254, 1);   //(gain, minsize, maxsize, gainlim
}

void loop()
{
   
  result = ircam.read();
  
  if (result & BLOB1)
  {
    Serial.print(" 1,");
    Serial.print(ircam.Blob1.X);
    Serial.print(",");
    Serial.print(ircam.Blob1.Y);
    Serial.print(",");
    Serial.print(ircam.Blob1.Size+1);
  }
  
  if (result & BLOB2)
  {
    Serial.print(" 2,");
    Serial.print(ircam.Blob2.X);
    Serial.print(",");
    Serial.print(ircam.Blob2.Y);
    Serial.print(",");
    Serial.print(ircam.Blob2.Size+1);
  }
  if (result & BLOB3)
  {
    Serial.print(" 3,");
    Serial.print(ircam.Blob3.X);
    Serial.print(",");
    Serial.print(ircam.Blob3.Y);
    Serial.print(",");
    Serial.print(ircam.Blob3.Size+1);
  }
  if (result & BLOB4)
  {
    Serial.print(" 4,");
    Serial.print(ircam.Blob4.X);
    Serial.print(",");
    Serial.print(ircam.Blob4.Y);
    Serial.print(",");
    Serial.print(ircam.Blob4.Size+1);
  }
  if (result) Serial.println();
  // Short delay...
  delay(30);
  

}
