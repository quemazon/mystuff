/*

*/
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
#define FAST_TURN 100    //100
#define SLOW_TURN 80     //80
#define FAST_FORWARD 254 
#define SLOW_FORWARD 80
#define MEDIUM_FORWARD 254

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
#define SHARP_LIM_FAR	210
#define SHARP_LIM_CLOSE	350
#define WHITE_THRESHOLD	200

//Initialize variables
unsigned int sensorValues[NUM_SENSORS];
boolean gyro_flag = false, cal_flag = false, long_flag = false;
long gyro_count = 0, gyro_null=0, accum=0, time=0, count, timeout = 0;
double x=0, y=0;
int angle_diff, angle_last, angle_target, angle_camera, angle=0;
int speed_current, speed_target, speed_turn, speed_ramp, speed_right, speed_left, steer_gain;
byte result, state, front_flags, side_flags;

//Initialize objects
MPU6050 accelgyro;
Servo escR;
Servo escL;
QTRSensorsRC qtrrc((unsigned char[]) {FR_LINE_PIN, FL_LINE_PIN, RC_LINE_PIN}, NUM_SENSORS, TIMEOUT, EMITTER_PIN); 

void setSpeed(int speed, int turn){
	Serial.println("speed");
	int speed_right = speed - turn;
	int speed_left = speed + turn;
	
	if (speed_right > 255) speed_right = 255;
	if (speed_left > 255) speed_left = 255;
	
	int temp = (speed_right * 2) + ESC_NULL;
	escR.writeMicroseconds(temp);
	temp = (speed_left * 2) + ESC_NULL;
	escL.writeMicroseconds(temp);
}

void tartgetAngle(int setangle){
	accum = 0;
	angle_target = setangle;
}

byte read_front(){
	//Serial.println("rfront");
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
	//Serial.println("rside");
	byte flags = 0;
	int temp = analogRead(LEFT_PIN);

	if (temp > SHARP_LIM_CLOSE) flags += LEFT_NEAR;
	else if (temp > SHARP_LIM_FAR) flags += LEFT_FAR;
	else flags += LEFT_NONE;

	temp = analogRead(RIGHT_PIN);
	if (temp > SHARP_LIM_CLOSE) flags += RIGHT_NEAR;
	else if (temp > 350) flags += RIGHT_FAR;
	else flags += RIGHT_NONE;
	return flags;
}

void decide_front(){
	//Serial.println("dfront");
	switch (front_flags) {
    case FR_NEAR+FL_NEAR: 
      setSpeed(FAST_FORWARD, 0);//case 1
	  accum = 0;
	  timeout=millis();
	  //Serial.println("Straight");
      break;
    case FR_FAR+FL_NEAR:
      setSpeed(MEDIUM_FORWARD, -SLOW_TURN);//case 2
	  //Serial.println("Straight");
	  accum = 0;
	  timeout=millis();
      break;
    case FR_NONE+FL_NEAR:
      setSpeed(MEDIUM_FORWARD, -FAST_TURN);//case 3
	  accum = GYRO_CAL/180*55;
	  timeout=millis();
	  //Serial.println("left");
      break;
    case FR_NEAR+FL_FAR:
      setSpeed(MEDIUM_FORWARD, SLOW_TURN);//case 4
	  timeout=millis();
	  //Serial.println("Straight");
	  accum = 0;
      break;
    case FR_FAR+FL_FAR:
      setSpeed(MEDIUM_FORWARD, 0);//case 5
	  timeout=millis();
	  //Serial.println("Straight");
	  accum = 0;
      break;
    case FR_NONE+FL_FAR:
      setSpeed(MEDIUM_FORWARD, -SLOW_TURN);//case 6
	  accum = GYRO_CAL/180*5;	  
	  timeout=millis();
	  //Serial.println("left");
      break;
    case FR_NEAR+FL_NONE:
      setSpeed(MEDIUM_FORWARD, FAST_TURN);//case 7
	  accum = -GYRO_CAL/180*5;
	  timeout=millis();
	  //Serial.println("right");
      break;
    case FR_FAR+FL_NONE:
      setSpeed(MEDIUM_FORWARD, SLOW_TURN);//case 8
	  accum = -GYRO_CAL/180*5;
	  timeout=millis();
	  //Serial.println("right");
      break;
    case FR_NONE+FL_NONE:
	  //Serial.println(-SLOW_TURN - angle);
	  if (accum > 0) setSpeed(0, -220);//case 9
      else setSpeed(0, 220);
	  break;
	}
}

void follow_line(){
	//setSpeed(150, -70);
	while(true){
		qtrrc.read(sensorValues);
		if ((sensorValues[0] < 100) || (sensorValues[1] < 100)){
			setSpeed(150,-120);
		}
		else setSpeed(250, -150);
	}
	
}

void decide_side(){
	//Serial.println("dside");
	switch (side_flags) {
    case LEFT_NEAR+RIGHT_NONE: 
	  accum = GYRO_CAL/180*90;
	  timeout=millis();
	  break;
    case LEFT_FAR+RIGHT_NONE: 
	  accum = GYRO_CAL/180*90;
	  timeout=millis();
	  break;
    case RIGHT_NEAR+LEFT_NONE: 
	  accum = -GYRO_CAL/180*90;
	  timeout=millis();
	  break;
    case RIGHT_FAR+RIGHT_NONE: 
	  accum = -GYRO_CAL/180*90;
	  timeout=millis();
	  break;
	}
}

void mydelay(int temp){
	//Serial.println("delay");
	for (int i = 0; i < temp; i++) {
		read_FIFO();
		delay(1);
	}
}

void decide_line(){
	//Serial.println("dline");
	if (sensorValues[0] < WHITE_THRESHOLD) {
		setSpeed(-FAST_FORWARD, 0);
		mydelay(70);
	}
	else if (sensorValues[1] < WHITE_THRESHOLD) {
		setSpeed(-FAST_FORWARD, 0);
		mydelay(70);
	}
	return;
}

byte watch_line(){
	qtrrc.read(sensorValues);
	Serial.print(sensorValues[0]);
    Serial.print('\t');
	Serial.print(sensorValues[1]);
    Serial.print('\t');
	Serial.println(sensorValues[2]);
	delay(250);
}

void watch_sensors(){
	Serial.print(analogRead(0));
	Serial.print("\t");
	Serial.print(analogRead(1));
	Serial.print("\t");
	Serial.print(analogRead(2));
	Serial.print("\t");
	Serial.println(analogRead(3));
	delay(300);
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
	accelgyro.resetFIFO();
	
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
	//Serial.println("FIFO");
	uint8_t buffer[2];
	long temp = 0;
	int samplz = 0;
	samplz = accelgyro.getFIFOCount() >> 1;
	//Serial.println(samplz,DEC);
	for(int i=0; i < samplz; i++){
		accelgyro.getFIFOBytes(buffer, 2);
		temp = ((((int16_t)buffer[0]) << 8) | buffer[1]);
		accum += temp;  // - gyro_null;    
		gyro_count++;
		
		//if((accum > GYRO_CAL) && (!cal_flag)) accum -= GYRO_CAL*2; //if we are calculating null, don't roll-over
		//if((accum < -GYRO_CAL) && (!cal_flag)) accum += GYRO_CAL*2;
	}
	//angle = (float)accum/(float)GYRO_CAL * -3.14159;   //change sign of PI for flipped gyro
	//angle = (float)accum/GYRO_CAL * -180;   //using degrees *10, negative for flipped gyro.

	return ;
}

void setup_mpu6050(){
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // use the code below to change accel/gyro offset values
    accelgyro.setXGyroOffset(85);  //85
    accelgyro.setYGyroOffset(-70);  //-70
    accelgyro.setZGyroOffset(46);  //-22
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // 
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // 
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 
    Serial.print("\n");
    
	Serial.println(F("Setting clock source to Z Gyro..."));
	accelgyro.setClockSource(MPU6050_CLOCK_PLL_ZGYRO);
	//Serial.println(accelgyro.getClockSource(MPU6050_CLOCK_PLL_ZGYRO);

	Serial.println(F("Setting sample rate to 200Hz..."));
	accelgyro.setRate(0); // 1khz / (1 + 4) = 200 Hz

 // *          |   ACCELEROMETER    |           GYROSCOPE
 // * DLPF_CFG | Bandwidth | Delay  | Bandwidth | Delay  | Sample Rate
 // * ---------+-----------+--------+-----------+--------+-------------
 // * 0        | 260Hz     | 0ms    | 256Hz     | 0.98ms | 8kHz
 // * 1        | 184Hz     | 2.0ms  | 188Hz     | 1.9ms  | 1kHz
 // * 2        | 94Hz      | 3.0ms  | 98Hz      | 2.8ms  | 1kHz
 // * 3        | 44Hz      | 4.9ms  | 42Hz      | 4.8ms  | 1kHz
 // * 4        | 21Hz      | 8.5ms  | 20Hz      | 8.3ms  | 1kHz
 // * 5        | 10Hz      | 13.8ms | 10Hz      | 13.4ms | 1kHz
 // * 6        | 5Hz       | 19.0ms | 5Hz       | 18.6ms | 1kHz
 // * 7        |   -- Reserved --   |   -- Reserved --   | Reserved

	Serial.println(F("Setting DLPF bandwidth"));
	accelgyro.setDLPFMode(MPU6050_DLPF_BW_42);

	Serial.println(F("Setting gyro sensitivity to +/- 250 deg/sec..."));
	accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
	//accelgyro.setFullScaleGyroRange(0);  // 0=250, 1=500, 2=1000, 3=2000 deg/sec

	Serial.println(F("Resetting FIFO..."));
	accelgyro.resetFIFO();

	Serial.println(F("Enabling FIFO..."));
	accelgyro.setFIFOEnabled(true);
	accelgyro.setZGyroFIFOEnabled(true);
	accelgyro.setXGyroFIFOEnabled(false);
	accelgyro.setYGyroFIFOEnabled(false);
	accelgyro.setAccelFIFOEnabled(false);
	Serial.print("Z axis enabled?\t"); Serial.println(accelgyro.getZGyroFIFOEnabled());
	Serial.print("x axis enabled?\t"); Serial.println(accelgyro.getXGyroFIFOEnabled());
	Serial.print("y axis enabled?\t"); Serial.println(accelgyro.getYGyroFIFOEnabled());
	Serial.print("accel enabled?\t"); Serial.println(accelgyro.getAccelFIFOEnabled());
	accelgyro.resetFIFO();
	return ;
}

void setup(){
	Wire.begin();
	Serial.begin(115200);
	setup_mpu6050();
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
	pinMode(9, INPUT_PULLUP);
	//while(digitalRead(9));
	//calculate_null();
	delay(3200);
	accelgyro.resetFIFO();
}

void loop(){
	//follow_line();
	//watch_gyro();
	//watch_sensors();
	qtrrc.read(sensorValues);
	decide_line();
	if ((millis() - timeout) > 500){
		accum = GYRO_CAL;
		timeout = millis();
	}
	if ((millis() - time) > 1){
		time = millis();
		read_FIFO();
		front_flags = read_front();
		side_flags = read_side();
		decide_side();
		decide_front();
	}
	//Serial.println(accum);
	//watch_line();
}