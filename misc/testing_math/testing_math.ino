volatile byte clicks = 0;
boolean manual, automatic, aux=false, running=false, first=true, gyro_flag = false, cal_flag = false;
long gyro_sum = 0, gyro_count = 0, angle_long=0, time=0, count;
double angle, angle_last, angle_target, proximity, steer_us, angle_diff, previous_proximity=10000;
int x_wp[10], y_wp[10];
double x=0, y=0;
int speed_cur=0, speed_new=0, speed_old=0, steer_limm = 300, gyro_null=0;
byte wpr_count=1, wpw_count=1;
const int InterruptPin = 2;		//intterupt on digital pin 2
byte wow[5];
byte* bptr;
int* iptr;

void print_values() {
	Serial.println(" ");
	Serial.print("double x: ");
	Serial.println(x);
	Serial.print("double y: ");
	Serial.println(y);
	Serial.print("byte clicks: ");
	Serial.println(clicks);
	Serial.print("long time: ");
	Serial.println(time);
	Serial.print("int x_wp[2] ");
	Serial.println(x_wp[2]);


}

void setup() {
		Serial.begin(115200);
}

void loop() {
	x=12345678;
	y=4.57e3;
	clicks = 100;
	time = 45678912;
	x_wp[2] = 45678912;
	print_values();
	
	x = x*y;
	y += clicks;
	time = time / clicks;
	clicks = 25.0 / 10.0;
	x_wp[2] = (int)((float)time / 10);
	
	print_values();
	
	x = atan2(5,4) * 180/3.14;
	y = sin((40+50) * 3.14159/360.0);
	//x_wp[2] = *x_wp[2];
	//time = (long)&x_wp[2];
	print_values();
	
	wow[0] = 45;
	wow[1] = 0;
	bptr = wow;
	Serial.println(*bptr);
	Serial.println(*(bptr+1));
	iptr = (int*)bptr;
	Serial.println(*iptr);
	iptr = (int*)wow;
	
	
	
	while(true) ;
}