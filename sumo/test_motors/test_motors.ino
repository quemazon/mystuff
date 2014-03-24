void setup()
{
  pinMode(2, OUTPUT);   // sets the pin as output
  pinMode(3, OUTPUT);   // sets the pin as output
  pinMode(4, OUTPUT);   // sets the pin as output
  pinMode(5, OUTPUT);   // sets the pin as output
  pinMode(6, OUTPUT);   // sets the pin as output
  pinMode(7, OUTPUT);   // sets the pin as output
}

void loop()
{
  while(true){
  digitalWrite(3, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
  analogWrite(2, 128);  // 
  analogWrite(7, 128);  //
  delay(1000); 
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
  delay(1000);
  }
}
