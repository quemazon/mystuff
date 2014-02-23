#include <Wire.h>
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include <I2Cdev.h>
//#include "MPU6050_6Axis_MotionApps20.h"
#include <MPU6050.h>
//#include "new_gyro.h"

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
	setup_mpu6050();
	//calculate_null();
}

void loop()
{
}
