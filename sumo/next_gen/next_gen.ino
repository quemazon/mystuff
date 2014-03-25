#include <Servo.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <QTRSensors.h>

#define NULL_FF -30
#define GYRO_CAL 23543420	//this has to be measured by rotating the gyro 360 deg. and reading the output

#define NUM_SENSORS   	3     // number of sensors used
#define TIMEOUT       	800  // waits for 2500 microseconds for sensor outputs to go low
#define EMITTER_PIN   	QTR_NO_EMITTER_PIN     // emitter is controlled by digital pin 2
#define ESC_NULL		1460

//speed definitions
#define FAST_TURN 100
#define SLOW_TURN 80
#define FAST_FORWARD 220 
#define SLOW_FORWARD 80
#define MEDIUM_FORWARD 190

//Pin definitions
#define FL_PIN A1
#define FR_PIN A2
#define RIGHT_PIN A3
#define LEFT_PIN A0
#define FR_LINE_PIN 3
#define FL_LINE_PIN 4
#define RC_LINE_PIN 5
#define R_ESC_PIN 2
#define L_ESC_PIN 7

//Front sensor defines
#define FL_NEAR 1
#define FL_FAR 2
#define FL_NONE 4
#define FR_NEAR 8
#define FR_FAR 16
#define FR_NONE 32

//Side sensor defines
#define RIGHT_NEAR 1
#define RIGHT_FAR 2
#define RIGHT_NONE 4
#define LEFT_NEAR 8
#define LEFT_FAR 16
#define LEFT_NONE 32

//Line sensor defines
#define FR_LINE 1
#define FL_LINE 2
#define RC_LINE 3

//sensor limits
#define SHARP_LIM_FAR	170
#define SHARP_LIM_CLOSE	300
#define WHITE_THRESHOLD	200

//Initialize variables
unsigned int sensorValues[NUM_SENSORS];
boolean gyro_flag = false, cal_flag = false, long_flag = false;
long gyro_count = 0, gyro_null=0, accum=0, time=0, count, last_millis;
double x=0, y=0;
int angle_diff, angle_last, angle_target, angle_camera, angle=0;
int speed_current, speed_target, speed_turn, speed_ramp, speed_right, speed_left, steer_gain;
byte result, state, flags, timeout=0;

//Initialize objects
MPU6050 accelgyro;
Servo escR;
Servo escL;
QTRSensorsRC qtrrc((unsigned char[]) {FR_LINE_PIN, FL_LINE_PIN, RC_LINE_PIN}, NUM_SENSORS, TIMEOUT, EMITTER_PIN); 

void setSpeed(int speed, int turn){
	int speed_right = speed - turn;
	int speed_left = speed + turn;
	
	if (speed_right > 255) speed_right = 255;
	if (speed_left > 255) speed_left = 255;
	
	int temp = (speed_right << 1) + ESC_NULL;
	escR.writeMicroseconds(temp);
	temp = (speed_left << 1) + ESC_NULL;
	escL.writeMicroseconds(temp);
}

byte read_front(){
	byte flags = 0;
	int temp = analogRead(FL_PIN);

	if (temp > SHARP_LIM_CLOSE) flags += FL_NEAR;
	else if (temp > SHARP_LIM_FAR) flags += FL_FAR;
	else flags += FL_NONE;

	temp = analogRead(FR_PIN);
	if (temp > SHARP_LIM_CLOSE) flags += FR_NEAR;
	else if (temp > SHARP_LIM_FAR) flags += FR_FAR;
	else flags += FR_NONE;
	return flags;
}

byte read_side(){
	byte flags = 0;
	int temp = analogRead(LEFT_PIN);

	if (temp > SHARP_LIM_CLOSE) flags += LEFT_NEAR;
	else if (temp > SHARP_LIM_FAR) flags += LEFT_FAR;
	else flags += LEFT_NONE;

	temp = analogRead(RIGHT_PIN);
	if (temp > SHARP_LIM_CLOSE) flags += RIGHT_NEAR;
	else if (temp > SHARP_LIM_FAR) flags += RIGHT_FAR;
	else flags += RIGHT_NONE;
	return flags;
}

void decide(){
	switch (read_front()) {
    case FR_NEAR+FL_NEAR: 
      setSpeed(FAST_FORWARD, 0);//case 1
      break;
    case FR_FAR+FL_NEAR:
      setSpeed(MEDIUM_FORWARD, -SLOW_TURN);//case 2
      break;
    case FR_NONE+FL_NEAR:
      setSpeed(MEDIUM_FORWARD, -FAST_TURN);//case 3
      break;
    case FR_NEAR+FL_FAR:
      setSpeed(MEDIUM_FORWARD, SLOW_TURN);//case 4
      break;
    case FR_FAR+FL_FAR:
      setSpeed(MEDIUM_FORWARD, 0);//case 5
      break;
    case FR_NONE+FL_FAR:
      setSpeed(MEDIUM_FORWARD, -SLOW_TURN);//case 6
      break;
    case FR_NEAR+FL_NONE:
      setSpeed(MEDIUM_FORWARD, FAST_TURN);//case 7
      break;
    case FR_FAR+FL_NONE:
      setSpeed(MEDIUM_FORWARD, SLOW_TURN);//case 8
      break;
    case FR_NONE+FL_NONE:
      setSpeed(0, SLOW_TURN);//case 9
      break;
	}
}

byte read_side();

byte read_line(){
	qtrrc.read(sensorValues);
	Serial.print(sensorValues[0]);
    Serial.print('\t');
	Serial.print(sensorValues[1]);
    Serial.print('\t');
	Serial.println(sensorValues[2]);
	delay(250);
}

void watch_angle(){
	Serial.println("watch angle");
	while(true) {
		read_FIFO();
		Serial.println(angle);  //watching in radians
		delay(30);
	}
}

void watch_gyro(){
	Serial.println();
	setup_mpu6050();
	calculate_null();

	Serial.println("watch gyro");
	do {
		read_FIFO();

		if((millis()-time)> 250){
			Serial.println(accum);
			time = millis();
		}
	} while(true);		//keep summing until we turn the mode switch off.

	return ;
}

void calculate_null(){
	Serial.println("CALCULATING NULL");

	cal_flag = true;		//tell ADC ISR that we are calibrating,
	accum = 0;				//reset the angle. angle will act as accumulator for null calculation
	gyro_null = 0;			//make sure to not subract any nulls here
	gyro_count = 0;

	while(gyro_count < 5000){
		read_FIFO();
		//delay(10);
		//Serial.println(gyro_count);
	}
	gyro_null = accum/gyro_count + NULL_FF;	//calculate the null. the -30 is a fudge factor for 5000 pts.
	cal_flag = false;		//stop calibration
	accum = 0;
	

	//should print null here
	Serial.print("Null: ");
	Serial.println(gyro_null);
	
	return ;
}

void read_FIFO(){
	uint8_t buffer[2];
	long temp = 0;
	int samplz = 0;
	samplz = accelgyro.getFIFOCount() >> 1;
	//Serial.println("FIFO_COUNTH : ");
	//Serial.println(samplz,DEC);
	for(int i=0; i < samplz; i++){
		accelgyro.getFIFOBytes(buffer, 2);
		temp = ((((int16_t)buffer[0]) << 8) | buffer[1]);
		accum -= temp + gyro_null;    
		gyro_count++;
		
		if((accum > GYRO_CAL) && (!cal_flag)) accum -= GYRO_CAL*2; //if we are calculating null, don't roll-over
		if((accum < -GYRO_CAL) && (!cal_flag)) accum += GYRO_CAL*2;
	}
	//angle = (float)accum/(float)GYRO_CAL * -3.14159;   //change sign of PI for flipped gyro
	angle = (float)accum/GYRO_CAL * -180;   //using degrees *10, negative for flipped gyro.

	return ;
}

void setup_mpu6050(){
	// initialize device
	Serial.println("Initializing I2C devices...");
	accelgyro.initialize();

	// verify connection
	Serial.println("Testing device connections...");
	Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
	
	// reset device
	Serial.println(F("\nResetting MPU6050..."));
	accelgyro.reset();
	delay(30); // wait after reset


	// disable sleep mode
	Serial.println(F("Disabling sleep mode..."));
	accelgyro.setSleepEnabled(false);

	// get X/Y/Z gyro offsets
	Serial.println(F("Reading gyro offset values..."));
	int8_t xgOffset = accelgyro.getXGyroOffset();
	int8_t ygOffset = accelgyro.getYGyroOffset();
	int8_t zgOffset = accelgyro.getZGyroOffset();
	Serial.print(F("X gyro offset = "));
	Serial.println(xgOffset);
	Serial.print(F("Y gyro offset = "));
	Serial.println(ygOffset);
	Serial.print(F("Z gyro offset = "));
	Serial.println(zgOffset);

	Serial.println(F("Setting clock source to Z Gyro..."));
	accelgyro.setClockSource(MPU6050_CLOCK_PLL_ZGYRO);

	Serial.println(F("Setting sample rate to 200Hz..."));
	accelgyro.setRate(0); // 1khz / (1 + 4) = 200 Hz

	Serial.println(F("Setting DLPF bandwidth to 42Hz..."));
	accelgyro.setDLPFMode(MPU6050_DLPF_BW_42);

	Serial.println(F("Setting gyro sensitivity to +/- 250 deg/sec..."));
	accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_250);

	Serial.println(F("Resetting FIFO..."));
	accelgyro.resetFIFO();

	Serial.println(F("Enabling FIFO..."));
	accelgyro.setFIFOEnabled(true);
	accelgyro.setZGyroFIFOEnabled(true);
	
	return ;
}

void setup(){
	Wire.begin();
	Serial.begin(115200);
	setup_mpu6050();
	calculate_null();
	pinMode(R_ESC_PIN, OUTPUT);   // sets the pin as output
	pinMode(L_ESC_PIN, OUTPUT);   // sets the pin as output
	pinMode(FL_PIN, INPUT);   // sets the pin as input
	pinMode(FR_PIN, INPUT);   // sets the pin as input
	pinMode(RIGHT_PIN, INPUT);   // sets the pin as input
	pinMode(LEFT_PIN, INPUT);   // sets the pin as input
	escR.attach(R_ESC_PIN);
	escL.attach(L_ESC_PIN);
	escR.writeMicroseconds(ESC_NULL);
	escL.writeMicroseconds(ESC_NULL);
	delay(1000);
}

void loop(){
	watch_angle();
}