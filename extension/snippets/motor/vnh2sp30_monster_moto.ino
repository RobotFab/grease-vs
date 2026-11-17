// snippet: vnh2sp30_monster_moto
// tags: motor, vnh2sp30, monster-moto, high-current, 30a, dc, sparkfun, direction, pwm
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Control 30A DC motor via SparkFun Monster Moto Shield (VNH2SP30)
// prompts: Monster Moto shield, VNH2SP30 30A motor, high current DC motor, SparkFun Monster Moto, big motor driver

const int INA1 = 7, INB1 = 8, PWM1 = 5, EN1 = 6;
const int INA2 = 4, INB2 = 9, PWM2 = 3, EN2 = 2;

void motor1(int speed) {
  if (speed > 0) { digitalWrite(INA1, HIGH); digitalWrite(INB1, LOW); }
  else           { digitalWrite(INA1, LOW);  digitalWrite(INB1, HIGH); speed = -speed; }
  analogWrite(PWM1, constrain(speed, 0, 255));
}

void motor2(int speed) {
  if (speed > 0) { digitalWrite(INA2, HIGH); digitalWrite(INB2, LOW); }
  else           { digitalWrite(INA2, LOW);  digitalWrite(INB2, HIGH); speed = -speed; }
  analogWrite(PWM2, constrain(speed, 0, 255));
}

void setup() {
  int pins[] = {INA1, INB1, PWM1, EN1, INA2, INB2, PWM2, EN2};
  for (int p : pins) pinMode(p, OUTPUT);
  digitalWrite(EN1, HIGH); digitalWrite(EN2, HIGH);
}

void loop() {
  motor1(200); motor2(200); delay(2000);
  motor1(0);   motor2(0);   delay(500);
  motor1(-180); motor2(-180); delay(2000);
  motor1(0);   motor2(0);   delay(500);
}
