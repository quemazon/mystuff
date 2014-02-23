#include <Servo.h>
#include <Wire.h>
#include <PVision.h>

//#include "Wire.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include <I2Cdev.h>
//#include "MPU6050_6Axis_MotionApps20.h"
#include <MPU6050.h>
//#include "new_gyro.h"

//motor directions
#define REVERSE 1
#define FORWARD 2
#define BRAKE 3

//pin definitions
#define PWM_R 5
#define PWM_L 10
#define DIR_R1 6
#define DIR_R2 7
#define DIR_L1 8
#define DIR_L2 9
#define SHARP_PIN 6

#define REV 1
#define FORW 2

// states
#define HUNT 1
#define APPROACH 2
#define ATTACK 3
#define HONING 4
#define LINE_DETECTED_GEN 5
#define LINE_DETECTED_RIGHT 6
#define LINE_DETECTED_LEFT 7
#define LINE_DETECTED_REAR 8

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
#define WHITE_THRESHOLD	800


#define NULL_FF -30
#define GYRO_CAL 235434205	//this has to be measured by rotating the gyro 360 deg. and reading the output

boolean gyro_flag = false, cal_flag = false, blob_flag = false, long_flag = false;
long gyro_count = 0, gyro_null=0, accum=0, time=0, count, last_millis;
double x=0, y=0;
int angle_diff, angle_last, angle_target, angle_camera, angle=0;
const byte InterruptPin = 2 ;		//intterupt on digital pin 2
int speed_current, speed_target, speed_turn, speed_ramp, speed_right, speed_left, steer_gain;
byte result, state, flags, timeout=0;

MPU6050 accelgyro;
PVision ircam;
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

void turnTime(int time, int speed, int turn) {
	long timetemp = millis() + time;
	speed_target = speed;
	speed_turn = turn;
	updateSpeed();
	while (millis() < timetemp) {
	}
}

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
	speed_target = speed_temp;
	speed_turn = turn;
	checkProcesses();
	//while (true) {
	while (!(((subtractAngles(angle_limH, angle)) > 0) && ((subtractAngles(angle, angle_limL)) >0))) {
		Serial.print("current angle\t");
		Serial.print(angle);
		Serial.print("\thigh angle\t");
		Serial.print(subtractAngles(angle_limH, angle));
		Serial.print("\tlow angle\t");
		Serial.println(subtractAngles(angle, angle_limL));
		delay(3);
		read_FIFO();
		checkProcesses();
	}
	Serial.println("exit turn");
}

void updateSpeed_bak(){
	if ((speed_target - speed_current) > 0){
		speed_current += speed_ramp;
		if (speed_current > speed_target) speed_current = speed_target - 1;
	}
	
	else{
		speed_current -= speed_ramp;
		if (speed_current <= speed_target) speed_current = speed_target + 1;
	}
	
	int speed_right = speed_current - speed_turn;
	int speed_left = speed_current + speed_turn;
	
	if (speed_right < 0) dirRight(REV);
	else dirRight(FORW);
	if (speed_left < 0) dirLeft(REV);
	else dirLeft(FORW);
	
	speed_right = abs(speed_right);
	speed_left = abs(speed_left);
	if (speed_right > 255) speed_right = 255;
	if (speed_left > 255) speed_left = 255;
	
	analogWrite(PWM_R, abs(speed_right));
	analogWrite(PWM_L, abs(speed_left));
}

void updateSpeed(){
	int speed_right = speed_target - speed_turn;
	int speed_left = speed_target + speed_turn;
	
	// if (speed_right < 0) dirRight(REV);
	// else dirRight(FORW);
	// if (speed_left < 0) dirLeft(REV);
	// else dirLeft(FORW);
	
	// speed_right = abs(speed_right);
	// speed_left = abs(speed_left);
	if (speed_right > 255) speed_right = 255;
	if (speed_left > 255) speed_left = 255;
	
	// analogWrite(PWM_R, abs(speed_right));
	// analogWrite(PWM_L, abs(speed_left));
	int temp = (speed_right << 1) + 1460;
	esc1.writeMicroseconds(temp);
	temp = (speed_left << 1) + 1460;
	esc2.writeMicroseconds(temp);
}

void dirRight(byte direction) {   //sets the direction of the right motor
	switch (direction) {
	case REV:
		digitalWrite(DIR_R1, HIGH);
		digitalWrite(DIR_R2, LOW);
		break;
	case FORW:
		digitalWrite(DIR_R1, LOW);
		digitalWrite(DIR_R2, HIGH);
		break;
	case BRAKE:
		digitalWrite(DIR_R1, HIGH);
		digitalWrite(DIR_R2, HIGH);
		break;
	}
}

void dirLeft(byte direction) {  //sets the direction of the left motor
	switch (direction) {
	case REV:
		digitalWrite(DIR_L1, HIGH);
		digitalWrite(DIR_L2, LOW);
		break;
	case FORW:
		digitalWrite(DIR_L1, LOW);
		digitalWrite(DIR_L2, HIGH);
		break;
	case BRAKE:
		digitalWrite(DIR_L1, HIGH);
		digitalWrite(DIR_L2, HIGH);
		break;
	}
}

boolean checkProcesses(){
	if ((millis() - last_millis) >= 10) {
		last_millis = millis();
		readCamera();
		read_FIFO();
		updateSpeed();
		return true;
	}
	return false;
}

void resetAngles() {
	angle_target = 0;
	angle_camera = 0;
	angle = 0;
	accum = 0;
	timeout = 0;
}

void readCamera(){
	result = ircam.read();
	if (result & BLOB1) {
		angle_camera = (ircam.Blob1.X-555) * .000735;   //calculates heading to target in radians
		flags += CAMERA;
	}
}

void testCamera() {
	byte i = 0;
	while (true) {
		//delay 500;
		//Serial.printls("cool");
		if (checkProcesses()) i++;
		if (i > 10){
			i = 0;
			Serial.println(angle_target);
			Serial.println(angle);
		}
	}
}

void testLine(){
	while (true) {
		Serial.print(analogRead(0));
		Serial.print(",");
		Serial.print(analogRead(1));
		Serial.print(",");
		Serial.println(analogRead(2));
		delay(100);
	}
}

void testLong(){
	byte i = 0;
	while (true) {
		//delay 500;
		//Serial.printls("cool");
		if (checkProcesses()) i++;
		if (i > 10){
			i = 0;
			Serial.println(analogRead(SHARP_PIN));
		}
	}
	
}

void testClose(){
	while(true) {
		Serial.println(digitalRead(A3));
		delay(100);
	}
}

void testTurn() {
	while(true) {
		Serial.println("turn1");
		turnAngle(90, 120, 60);
		dirRight(BRAKE);
		dirLeft(BRAKE);
		delay(50);
		Serial.println("turn2");
		turnAngle(-90, 120, 60);
		dirRight(BRAKE);
		dirLeft(BRAKE);
		delay(50);
	}
}

byte hunt_bak(){
	speed_current = 0;
	speed_target = 0;
	speed_turn = 0;
	while(true) {
		flags = 0;
		checkProcesses();
		readLong();
		readLine();
		if (flags & SHARP_FAR) return APPROACH;
		if (flags & SHARP_CLOSE) return ATTACK;
		if (flags & CAMERA) return HONING;
		if (flags & (LINE_FR | LINE_FL | LINE_R)) return LINE_DETECTED_GEN;
	}
}

byte hunt(){
	Serial.println("hunt");
	speed_current = 0;
	speed_target = 0;
	speed_turn = 0;
	while(true) {
		flags = 0;
		checkProcesses();
		readLong();
		readClose();
		readLine();
		if (flags & SHARP_HERE) return ATTACK;
		if (flags & (LINE_FR | LINE_FL | LINE_R))return LINE_DETECTED_GEN;
		if (flags & SHARP_CLOSE) return APPROACH;
		if (flags & SHARP_FAR) return APPROACH;
		if (flags & CAMERA) return HONING;
	}
}

byte honing(){
	Serial.println("honing");
	speed_current = 0;
	speed_target = 0;
	speed_turn = 0;
	while(true) {
		flags = 0;
		checkProcesses();
		readLong();
		readClose();
		readLine();
		if (flags & SHARP_HERE) return ATTACK;
		if (flags & (LINE_FR | LINE_FL | LINE_R))return LINE_DETECTED_GEN;		
		if (flags & SHARP_FAR) return APPROACH;
		if (flags & SHARP_CLOSE) return APPROACH;
		speed_turn = angle_target * 250;
	}
}

byte approach2(){
	int counter = 500;		//this variable counts-down every time the camera doesn't see anything eventually it times-out
	while(counter) {
		flags = 0;
		checkProcesses();	//1. read gyro 2. update speed 3. read camera
		readLong();
		readLine();
		if (flags & SHARP_FAR) speed_target = 100;
		else if (flags & SHARP_CLOSE) return ATTACK;
		else speed_target = 0;
		if (LINE_FR || LINE_FL|| LINE_R) return LINE_DETECTED_GEN;
		if (flags & CAMERA) {
			speed_turn = steer_gain * (angle_target - angle);
			counter = 50;
		}
		else counter -= 1;
	}
	return HUNT;  // if counter reaches 0, nothing has been seen for a while, to go back to hunting.
}

byte approach(){
	Serial.println("approach");
	speed_current = 0;
	speed_target = 100;
	speed_turn = 0;
	resetAngles();
	while(true) {
		flags = 0;
		checkProcesses();
		readLong();
		readClose();
		readLine();
		speed_turn = angle_target * 150;
		if (flags & SHARP_HERE) return ATTACK;
		if (flags & (LINE_FR | LINE_FL | LINE_R))return LINE_DETECTED_GEN;		
		if (flags & SHARP_FAR) speed_target = 100;
		else if (flags & SHARP_CLOSE) speed_target = 200;
		else return HUNT;
		//if (flags & CAMERA) return HONING;
		//if (flags & LINE_FR & LINE_FL & LINE_R) return LINE_DETECTED;
	}
}

byte attack() {
	Serial.println("attack");
	speed_current = 0;
	speed_target = 254;
	speed_turn = 0;
	while(true) {
		flags = 0;
		checkProcesses();
		readClose();
		if (!(flags & SHARP_HERE)) return HUNT;
	}
	
}

void readLong(){			//reads the long range sharp sensor. "flags" should be cleared before calling
	int temp = analogRead(SHARP_PIN);
	if (temp > SHARP_LIM_CLOSE) flags += SHARP_CLOSE;
	else if (temp > SHARP_LIM_FAR) flags += SHARP_FAR;
	// else nothing seen, then nothing
}

void readClose() {
	if (!(digitalRead(A3))) flags += SHARP_HERE;
}

void readLine(){
	if (analogRead(0) > WHITE_THRESHOLD) flags += LINE_FR;
	if (analogRead(1) > WHITE_THRESHOLD) flags += LINE_FL;
	if (analogRead(2) > WHITE_THRESHOLD) flags += LINE_R;
}

byte lineDetected(){
	Serial.println("line detected");
	if (flags & LINE_FR) return LINE_DETECTED_RIGHT;
	if (flags & LINE_FL) return LINE_DETECTED_LEFT;
	if (flags & LINE_R) return LINE_DETECTED_REAR;
}

byte lineDetectedRight() {
	Serial.println("right line detected");
	//speed_current = 0;
	speed_target = -70;
	speed_turn = 30;
	updateSpeed();
	delay (400);
	flip();
	speed_target = 0;
	speed_turn = 0;
	updateSpeed();
	return HUNT;
}

byte lineDetectedLeft() {
	Serial.println("left line detected");
	//speed_current = 0;
	speed_target = -70;
	speed_turn = -30;
	updateSpeed();
	delay (400);
	flip();
	speed_target = 0;
	speed_turn = 0;
	updateSpeed();
	return HUNT;
}

byte lineDetectedRear() {
	Serial.println("rear line detected");
	//speed_current = 0;
	speed_target = 70;
	speed_turn = 0;
	updateSpeed();
	delay (400);
	flip();
	speed_target = 0;
	speed_turn = 0;
	updateSpeed();
	return HUNT;
}

void flip() {
	speed_target = 0;
	speed_turn = 200;
	updateSpeed();
	delay(200);
}

void testMotors() {
	speed_target = 230;
	for (int i = 0; i < 50; i++) {
		updateSpeed();
		delay(10);
	}
	speed_target = 0;
	for (int i = 0; i < 50; i++) {
		updateSpeed();
		delay(10);
	}
	speed_target = -230;
	for (int i = 0; i < 50; i++) {
		updateSpeed();
		delay(10);
	}
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
	} while(true);		//keep summing unitil we turn the mode switch off.

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
		accum -= (temp * 10) + gyro_null;    
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

void initialize() {
  int counter = 0;
  digitalWrite(13, HIGH);
  Serial.println("waiting command");
  while (digitalRead(A3));
  Serial.println("one");
  delay(100);
  while (digitalRead(A3));
  Serial.println("two");
  delay(2000);
  digitalWrite(13, LOW);
  while (!(digitalRead(A3)));
  digitalWrite(13, HIGH);
  Serial.println("go!");
  delay(4800);
  digitalWrite(13, LOW);
}  

void setup() {
	ircam.init(75, 1, 254, 1);
	Serial.begin(115200);
	esc1.attach(2);
	esc2.attach(3);
	setup_mpu6050();
	calculate_null();
	pinMode(PWM_R, OUTPUT);   // sets the pin as output
	pinMode(PWM_L, OUTPUT);   // sets the pin as output
	pinMode(DIR_R1, OUTPUT);   // sets the pin as output
	pinMode(DIR_R2, OUTPUT);   // sets the pin as output
	pinMode(DIR_L1, OUTPUT);   // sets the pin as output
	pinMode(DIR_L2, OUTPUT);   // sets the pin as output
	pinMode(A3, INPUT);
	pinMode(A7, INPUT);
	//initialize();
	state = HUNT;
	speed_ramp = 60;
	//watch_angle();
	testTurn();


}

void loop()
{
	//  readReflectorValues();
	//  sensors.read(sensor_values);
	//watch_angle();
	//testLong();
	//testCamera();
	//speed_current = 0;
	//while(true) testMotors();
	//testLine();
	//testMotors();
	//testClose();
	
	switch (state) {
	case HUNT:			//No sensors see anything. move until a sensor picks something up
		state = hunt();
		break;

	case HONING:			//No sensors see anything. move until a sensor picks something up
		state = honing();
		break;

	case APPROACH:
		state = approach();
		break;
		
	case ATTACK:
		state = attack();
		break;

	case LINE_DETECTED_GEN:
		state = lineDetected();
		break;
		
	case LINE_DETECTED_RIGHT:
		state = lineDetectedRight();
		break;
		
	case LINE_DETECTED_LEFT:
		state = lineDetectedLeft();
		break;

		case LINE_DETECTED_REAR:
		state = lineDetectedRear();
		break;
		/*		
		
	case SWITCH_ATTACK:
		state = switchAttack();
		break;
	case SWITCH_ATTACK_REAR:
		state = switchAttackRear();
		break;
	case ATTACK_1:
		state = attack1();
		break;  */
	}
}
