#define SAMPLZ 1
long accum[4], output[4];
long time=0;
byte counter = 0;

void print_values(){
	Serial.print(output[0]);
	Serial.print("\t");
	Serial.print(output[1]);
	Serial.print("\t");
	Serial.print(output[2]);
	Serial.print("\t");
	Serial.println(output[3]);
}

void sample(){
	for (int i = 0; i <4; i++) accum[i] += analogRead(i);
	counter++;
}

void setup() {
	Serial.begin(115200);
}

void loop() {
	sample();
	if (counter == SAMPLZ){
		counter = 0;
		for (int i = 0; i < 4; i++){
			output[i] = accum[i]/SAMPLZ;
			accum[i] = 0;
		}
	}
	
	if ((millis() - time) > 500) {
		time = millis();
		print_values();
	}
}