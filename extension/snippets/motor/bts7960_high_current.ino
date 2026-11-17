// snippet: bts7960_high_current
// tags: motor, bts7960, high-current, 43a, pwm, dc, ibbt, direction, speed
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Control high-current DC motor with BTS7960 (43A) H-bridge — dual PWM pins
// prompts: BTS7960 motor driver, 43A DC motor, IBT-2 high current motor, high power H-bridge, large motor driver

const int RPWM = 5;   // forward PWM
const int LPWM = 6;   // reverse PWM
const int R_EN = 7;   // right enable
const int L_EN = 8;   // left enable

void motorForward(int speed) {
  analogWrite(RPWM, constrain(speed, 0, 255));
  analogWrite(LPWM, 0);
}

void motorBackward(int speed) {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, constrain(speed, 0, 255));
}

void motorStop() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);
}

void setup() {
  pinMode(RPWM, OUTPUT); pinMode(LPWM, OUTPUT);
  pinMode(R_EN, OUTPUT); pinMode(L_EN, OUTPUT);
  digitalWrite(R_EN, HIGH); digitalWrite(L_EN, HIGH);
}

void loop() {
  motorForward(200); delay(2000);
  motorStop();       delay(500);
  motorBackward(150); delay(2000);
  motorStop();       delay(500);
}
