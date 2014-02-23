int ledPin = 9;      // LED connected to digital pin 9
int analogPin = 3;   // potentiometer connected to analog pin 3
int val = 0;         // variable to store the read value

void setup()
{
  pinMode(5, OUTPUT);   // sets the pin as output
  pinMode(10, OUTPUT);   // sets the pin as output
  pinMode(6, OUTPUT);   // sets the pin as output
  pinMode(7, OUTPUT);   // sets the pin as output
  pinMode(8, OUTPUT);   // sets the pin as output
  pinMode(9, OUTPUT);   // sets the pin as output
}

void loop()
{
  while(true){
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);
  analogWrite(5, 128);  // 
  analogWrite(10, 128);  //
  delay(1000); 
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);
  delay(1000);
  }
}
