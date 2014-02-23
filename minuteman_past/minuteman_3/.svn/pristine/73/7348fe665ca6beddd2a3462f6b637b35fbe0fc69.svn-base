//gyro test & incorporation
/*Pin Assignments:
A0 - gyro
A1 - thermistor
D5 - ST1
D6 - ST2
*/

#define ST1 5
#define ST2 6
#define G_THERM 1
#define G_RATE 0

float voltage = 0.0, temp_c = 0.0, temp_f = 0.0;
int analog_read = 0;

void setup() {
	Serial.begin(115200);
	pinMode(ST1, OUTPUT);
	pinMode(ST2, OUTPUT);
	delay(1000);
}

void loop() {
//	self_test();
	temperature();
	delay(125);
}

void self_test(void) {

	for(int i=0; i<10; i++) {
		digitalWrite(ST1, LOW);
		digitalWrite(ST2, HIGH);
		gyro();	
	}
	
	for(int i=0; i<10; i++) {
		digitalWrite(ST1, HIGH);
		digitalWrite(ST2, LOW);
		gyro();
	}
	
	return ;
}

void gyro(void) {
	Serial.print(analogRead(G_RATE));
	Serial.print(", ");
	delay(100);
	return ;
}

void temperature(void) {
	analog_read = analogRead(G_THERM);
	voltage = 5.0*analog_read/1028.0;
	temp_c = (voltage - 2.275)/.009;
	temp_f = 9.0/5.0*temp_c + 32.0;

	Serial.print(analog_read);
	Serial.print(", ");
	Serial.print(voltage);
	Serial.print(", ");
	Serial.print(temp_c);
	Serial.print(", ");
	Serial.println(temp_f);

	return ;
}
