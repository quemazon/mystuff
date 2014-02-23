#include <Servo.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <QTRSensors.h>

#define NUM_SENSORS   	3     // number of sensors used
#define TIMEOUT       	800  // waits for 2500 microseconds for sensor outputs to go low
#define EMITTER_PIN   	QTR_NO_EMITTER_PIN     // emitter is controlled by digital pin 2
#define ESC_NULL		1460

#define NULL_FF -30			//fudge factor for the gyro null. probably need to change.
#define GYRO_CAL 235434205	//this has to be measured by rotating the gyro 360 deg. and reading the output

//sensor flags
#define LINE_FR		    1
#define LINE_FL		    2
#define LINE_R		    4
#define CAMERA			8
#define SHARP_FAR		16
#define SHARP_CLOSE		32
#define SHARP_HERE		64

//sensor limits
#define SHARP_LIM_FAR	120
#define SHARP_LIM_CLOSE	300
#define WHITE_THRESHOLD	200

//pin definitions
#define QTR_FR			8
#define QTR_FL			10
#define QTR_R			9
#define SHARP_PIN		A6

//Global Declarations
boolean cal_flag = false;
long gyro_count = 0, gyro_null=0, accum=0, time=0, count, last_millis;
int angle_diff, angle_last, angle_target, angle_camera, angle=0, speed;
int steer_gain;
byte result, state, flags, timeout=0;

//Initialize objects
QTRSensorsRC qtrrc((unsigned char[]) {8, 9, 10}, NUM_SENSORS, TIMEOUT, EMITTER_PIN); 
unsigned int sensorValues[NUM_SENSORS];

MPU6050 accelgyro;
Servo esc1;
Servo esc2;

int addAngles(long angle1, long angle2) {
	int temp = angle1 + angle2;
	if (temp > 180) temp -= 360;
	if (temp < -180) temp += 360;
	return temp;
}

int subtractAngles(long angle1, long angle2) {
	int temp = angle1 - angle2;
	if (temp > 180) temp -= 360;
	if (temp < -180) temp += 360;
	return temp;
}

// void turnTime(int time, int speed, int turn) {
	// long timetemp = millis() + time;
	// speed_target = speed;
	// speed_turn = turn;
	// updateSpeed();
	// while (millis() < timetemp) {
	// }
// }

void turnAngle(int angle_target, int speed_temp, int turn){
	if (angle_target < 0) turn = 0-turn;
	angle_target = addAngles(angle_target, angle);
	// Serial.print("initial angle:\t");
	// Serial.print(angle);
	// Serial.print("\ttarget angle:\t");
	// Serial.print(angle_target);
	int angle_limH = addAngles(angle_target, 5);
	int angle_limL = subtractAngles(angle_target, 5);
	// Serial.print("\tlim high:\t");
	// Serial.print(angle_limH);
	// Serial.print("\tlim low:\t");
	// Serial.println(angle_limL);
	setSpeed(speed_temp, turn);
	//while (true) {
	while (!(((subtractAngles(angle_limH, angle)) > 0) && ((subtractAngles(angle, angle_limL)) >0))) {
		Serial.print("current angle\t");
		Serial.print(angle);
		Serial.print("\thigh angle\t");
		Serial.print(subtractAngles(angle_limH, angle));
		Serial.print("\tlow angle\t");
		Serial.println(subtractAngles(angle, angle_limL));
		delay(3);
	}
		read_FIFO();
	Serial.println("exit turn");
}


void readLine(){
	qtrrc.read(sensorValues);
	if (sensorValues[0] < WHITE_THRESHOLD) flags += LINE_FR;
	if (sensorValues[1] < WHITE_THRESHOLD) flags += LINE_FL;
	//if (sensorValues[2] < WHITE_THRESHOLD) flags += LINE_R;
}

void setSpeed(int speed, int turn){
	int speed_right = speed - turn;
	int speed_left = speed + turn;
	
	if (speed_right > 255) speed_right = 255;
	if (speed_left > 255) speed_left = 255;
	
	int temp = (speed_right << 1) + ESC_NULL;
	esc1.writeMicroseconds(temp);
	temp = (speed_left << 1) + ESC_NULL;
	esc2.writeMicroseconds(temp);
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
		accum -= (temp * 10) + gyro_null;    
		gyro_count++;
		
		if((accum > GYRO_CAL) && (!cal_flag)) accum -= GYRO_CAL*2; //if we are calculating null, don't roll-over
		if((accum < -GYRO_CAL) && (!cal_flag)) accum += GYRO_CAL*2;
	}
	//angle = (float)accum/(float)GYRO_CAL * -3.14159;   //change sign of PI for flipped gyro
	angle = (float)accum/GYRO_CAL * -180;   //using degrees *10, negative for flipped gyro.

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

void setup() {
	Wire.begin();
	Serial.begin(115200);
	esc1.attach(2);
	esc2.attach(3);
	esc1.writeMicroseconds(ESC_NULL);
	esc2.writeMicroseconds(ESC_NULL);
	delay(1000);
	setup_mpu6050();
	calculate_null();
	while(true){
		Serial.print(analogRead(0));
		Serial.print("\t");
		Serial.print(analogRead(1));
		Serial.print("\t");
		Serial.print(analogRead(2));
		Serial.print("\t");
		Serial.println(analogRead(3));
		delay(300);
	}
}

void loop() {
	setSpeed(100, -46);
	do {
		flags = 0;
		readLine();
	} while (!(flags));
	
	setSpeed(100, -55);
	delay(200);
	// do {
		// flags = 0;
		// readLine();
	// } while (flags);
}

