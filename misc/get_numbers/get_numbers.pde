
long get_num() {
	char num[20];
	byte tmp;
	Serial.flush();
	for (byte i = 0; i < 20; i++) {
		while (Serial.available() == 0);
		tmp = Serial.read();
		Serial.print(tmp);
		if (tmp == '\r') return atol(num);
		num[i] = tmp;
		num[i+1] = 0;
	}
	return 0;
}


void setup() {
  // put your setup code here, to run once:
	Serial.begin(9600);
}

void loop() {
	//while (true) ;
  // put your main code here, to run repeatedly:	
	Serial.print("the number is: ");
	Serial.println(get_num());
}
