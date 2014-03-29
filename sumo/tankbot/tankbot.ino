int turn_count = 0;
boolean turn_direction = true;

void go_fast() {		// You have a different 
  analogWrite(2, 254);  // 
  analogWrite(7, 254);  //
}
	
void go_medium() {
  analogWrite(2, 128);  // 
  analogWrite(7, 128);  //
}
	
void go_slow() {
  analogWrite(2, 128);  // 
  analogWrite(7, 128);  //
}
	
void forward(){  
  digitalWrite(3, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
}

void backward(){
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
 }

void rotate_right(){
  digitalWrite(3, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
}

void rotate_left(){
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
  go_slow();
}



void reverse_rotation() {
	turn_direction = !turn_direction;
}

void setup()
{
  pinMode(2, OUTPUT);   // sets the pin as output
  pinMode(3, OUTPUT);   // sets the pin as output
  pinMode(4, OUTPUT);   // sets the pin as output
  pinMode(5, OUTPUT);   // sets the pin as output
  pinMode(6, OUTPUT);   // sets the pin as output
  pinMode(7, OUTPUT);   // sets the pin as output
  delay(3400);
}

void loop(){
	if (analogRead(0) > 150) {
		forward();
		go_fast();
		turn_count++;
	}
	
	else if (analogRead(1) > 700) {
		go_medium();
		backward();
		delay(300);
		rotate_left();
		go_slow();
	}
	
	else if (analogRead(2) >700) {
		go_medium();
		backward();
		delay(300);
		rotate_right;
		go_slow();
	}

	else{
		if (turn_count > 0) turn_direction = !turn_direction;
		turn_count = 0;
		if (turn_direction) rotate_right();
		else rotate_left();
		go_slow();
	}
}
