//gyro verification test
//Pin assignment: A0 - analog input from gyro
#define GYRO_LIMIT 1000 // defines how many gyro samples are taken between angle calculations
//calibration results: last year: 8741543, this year 3x: 8717450, this year 5x: 8708331, nathan's: 8650000
//average calibration number: 8712891
//null number calc'd: 479737, 479771, 479695 <- these are comparable to nathan's

volatile boolean new_cycle = false;	//volatile is use for interrupt service routines (ISRs)
volatile long gyro_sum = 0, gyro_count = 0, gyro_null=0, angle=0;
boolean aux=1;	//this enables calibration

void setup() {
	Serial.begin(115200);
	set_gyro_adc();						// sets up free running ADC for gyro
	Serial.println("gyro null calculation begin");
	calculate_null();
	Serial.println("gyro calibration start");
}

void loop() {
	calibrate_gyro();
}

void calibrate_gyro() {
	new_cycle = false;	//calibration needs to start on new cycle
	while (!new_cycle);	//wait for start of new cycle
		angle = 0;		//resets the angle
	do {
		Serial.println(angle);
		delay(250);
	} while (aux == 1);	//keep summing until we turn the mode switch off, angle will not roll-over
	return;
}

void set_gyro_adc() {
	ADMUX = B01000000;	//completely resets the MUX. should be sampling only on A0, set vref to internal
	ADCSRA = B11101111;	//set scaler, auto trigger, sampling, etc
	sei();
	Serial.println("gyro adc set");
	return;
}

ISR(ADC_vect) {        //ADC interrupt
	gyro_sum += ADCL | (ADCH << 8);
	gyro_count++;        //iterate the counter

	if (gyro_count == GYRO_LIMIT) {
		angle += (gyro_sum - gyro_null);
		gyro_sum = 0;
		gyro_count = 0;
		new_cycle = true;
	}
} 

void calculate_null() {				// only used once for cal of null...should be used at each waypoint
	new_cycle = false;				// this will be set, already, but need to begin on new cycle
	while (!new_cycle) ;			// wait for start of new cycle. this notation means "true if 'x' is false. so...while(!true) -> don't do the statement. while(!false) -> do the statement.
	angle = 0;						// reset the angle. angle will act as accumulator for null calculation
	gyro_null = 0;					// make sure to not subract any nulls here
	for (int i=0; i <= 50; i++) {
		while (!new_cycle);
		new_cycle = false;			// start another round
	}
	gyro_null = angle/50;			// calculate the null. up to this point, gyro null is 0, so nothing is being subtracted. therefore, angle is continually summing up.
	angle = 0;
	Serial.println("gyro null calculation complete");
	Serial.print("gyro null: ");
	Serial.println(gyro_null);
	return;
}