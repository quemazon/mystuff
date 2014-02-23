
// pins for the LEDs:
const int close1 = 4;   //10 cm sensor
const int close2 = 5;   //10 cm sensor
//const int far1 = 4;    //10-80 cm sensor
//const int far2 = 5;    //10-80 cm sensor

void setup() {
  // initialize serial:
  Serial.begin(115200);    // start the serial port
  // make the pins outputs:
  pinMode(close1, INPUT);   // the close sensors are digital
  pinMode(close2, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);

  
}

void loop() {
  int val[4];
 
  val[0] = analogRead(A0);    // the far sensors are analog
  val[1] = analogRead(A1);
  val[2] = analogRead(A2);
  val[3] = analogRead(A3);;
  
  
  Serial.print(val[0]);
  Serial.print("\t");      // this prints a tab
  Serial.print(val[1]);
  Serial.print("\t");
  Serial.print(val[2]);
  Serial.print("\t");
  Serial.println(val[3]);  //serial.println make a new line

  delay(300);   // wait 100 ms between loops
 }
