// snippet: l9110s_dual_motor
// tags: motor, l9110s, dual, dc, pwm, direction, robot, cheap, hg7881
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Drive two DC motors with L9110S (HG7881) dual H-bridge — no enable pin needed
// prompts: L9110S dual motor driver, HG7881 H-bridge, two DC motors cheap, robot drive L9110, dual H-bridge no enable

// Motor A: forward = A1A HIGH / A1B LOW, reverse = A1A LOW / A1B HIGH
const int A1A = 5;  // PWM
const int A1B = 6;  // PWM
const int B1A = 9;  // PWM
const int B1B = 10; // PWM

void motorA(int speed) {  // -255 to 255
  if (speed >= 0) { analogWrite(A1A, speed); analogWrite(A1B, 0); }
  else            { analogWrite(A1A, 0); analogWrite(A1B, -speed); }
}

void motorB(int speed) {
  if (speed >= 0) { analogWrite(B1A, speed); analogWrite(B1B, 0); }
  else            { analogWrite(B1A, 0); analogWrite(B1B, -speed); }
}

void setup() {
  pinMode(A1A, OUTPUT); pinMode(A1B, OUTPUT);
  pinMode(B1A, OUTPUT); pinMode(B1B, OUTPUT);
}

void loop() {
  motorA(200); motorB(200); delay(1500);  // forward
  motorA(0);   motorB(0);   delay(500);   // stop
  motorA(-150); motorB(-150); delay(1500); // reverse
  motorA(0);   motorB(0);   delay(500);
}
