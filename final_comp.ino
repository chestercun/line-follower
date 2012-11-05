// debugging
boolean debug_sensor = false;
// sensor readings (0-1023)
int t0, t1, t2, t3, t4, t5;
// sensor standards (0-1023)
int white_A2;
int white_A3;
// motor digital io
const int left_forward = 11;
const int left_reverse = 10;
const int right_forward = 6;
const int right_reverse = 5;

// establish a digital "standard"
void calibrate() {
  // A0-A1-A2-----A3-A4-A5
  white_A2 = analogRead(A2);
  white_A3 = analogRead(A3);
  delay(3000); // short pause
}

// control variables
// duty is "max" speed
int duty = 255; // PWM duty cycle (0-255) maps to (0%-100%)
// right motor seems to be faster than left
int compensation = 4;
// the minimum diff_diff for "non-straight"
int threshold = 6;
// speed variable
int s; // new speed

void motor_logic() {
  // compare to digital standards
  int diff_A2 = abs(white_A2-t2);
  int diff_A3 = abs(white_A3-t3);
  int diff_diff = abs(diff_A2-diff_A3);
  
  // if they're both black, go straight
  if (diff_A2 > 200 && diff_A3 > 200) {
    set_right_motor(255-compensation, 0);
    set_left_motor(255, 0);
    return;
  }
  // is the difference negligible?
  if (diff_diff < threshold) {
    // if so then go straight
    set_right_motor(duty-compensation, 0);
    set_left_motor(duty, 0);
  }
  
  // calculate lag
  if (diff_diff > 300) {
    diff_diff = 300; // capped max
  } else if (diff_diff <=6 ) {
    diff_diff = 6; // capped min
  }
  s = 255 - ((diff_diff-6)*85/49); // 510 over 294
  
  // debugging
  /*
  Serial.println(' ');
  Serial.print(diff_A2);
  Serial.print("\t");
  Serial.print(diff_A3);
  Serial.print("\t");
  Serial.print(diff_diff);
  Serial.print("\t");
  Serial.print(s);
  Serial.println(' ');
  //*/
  
  // adjust speeds
  if (diff_A2 > diff_A3) { // left turn
    set_right_motor(duty-compensation, 0);
    if (s > 0) {
      set_left_motor(s, 0);
    } else {
      set_left_motor(0,abs(s));
    }
  } else if (diff_A2 < diff_A3) { // right turn
    if (s > 0) {
      set_right_motor(s-compensation, 0);
    } else {
      set_right_motor(0,abs(s));
    }
    set_left_motor(duty, 0);
  } else {
    // just go straight
    set_right_motor(duty-compensation, 0);
    set_left_motor(duty, 0);
  }
}

// helpers
void set_right_motor(int duty1, int duty2) {
  analogWrite(right_forward, duty1);
  analogWrite(right_reverse,duty2);
}
void set_left_motor(int duty1, int duty2) {
  analogWrite(left_forward,duty1);
  analogWrite(left_reverse,duty2);
}

// Initialization
void setup() {
  // motor pins
  pinMode(right_forward,OUTPUT);
  pinMode(right_reverse,OUTPUT);
  pinMode(left_forward,OUTPUT);
  pinMode(left_reverse,OUTPUT);
  // for debugging
  Serial.begin(9600);
  // set digital standards
  calibrate();
}

// infinite loop
void loop() {
  read_sensors();
  motor_logic();
  //run_debug_logic();
  
  // let ADC settle
  delay(125);
}

// read QRD1114's
void read_sensors() {
  t2 = analogRead(A2);
  t3 = analogRead(A3);
}

// debug info - Serial
void run_debug_logic() {
  if (debug_sensor) {
    Serial.println(' ');
    Serial.print(t2);
    Serial.print("\t");
    Serial.print(t3);
    Serial.print("\t");
    Serial.println(' ');
  }
}


