#include <Servo.h>
#include <QTRSensors.h>

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

//Initialize objects
QTRSensorsRC qtrrc((unsigned char[]) {FR_LINE_PIN, FL_LINE_PIN, RC_LINE_PIN}, NUM_SENSORS, TIMEOUT, EMITTER_PIN); 
unsigned int sensorValues[NUM_SENSORS];

Servo escR;
Servo escL;

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

byte read_line();

void setup(){
	Serial.begin(115200);
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
	qtrrc.read(sensorValues);
	Serial.print(sensorValues[0]);
    Serial.print('\t');
	Serial.print(sensorValues[1]);
    Serial.print('\t');
	Serial.println(sensorValues[2]);
	delay(250);
	}