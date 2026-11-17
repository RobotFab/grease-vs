// snippet: tb6612_basic
// tags: motor, tb6612, tb6612fng, dc, driver, dual, h-bridge, pwm
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none (raw GPIO)
// description: Dual DC motor control with a TB6612FNG breakout using raw PWM — no library needed
// prompts: TB6612FNG motor driver, TB6612 DC motor, dual H-bridge TB6612, two motor raw PWM, TB6612 breakout
// not-tags: drv8833, l298n, l9110s, bts7960, adafruit-motor-shield

// TB6612FNG: AIN1/AIN2 + BIN1/BIN2 set direction (digital), PWMA/PWMB set speed (PWM),
// STBY must be HIGH to enable the driver. PWMA/PWMB must be PWM-capable pins.
const int AIN1 = 7, AIN2 = 8, PWMA = 5;   // Motor A
const int BIN1 = 4, BIN2 = 2, PWMB = 6;   // Motor B
const int STBY = 9;

void motorA(int speed) {  // speed: -255..255
  digitalWrite(AIN1, speed >= 0 ? HIGH : LOW);
  digitalWrite(AIN2, speed >= 0 ? LOW  : HIGH);
  analogWrite(PWMA, constrain(abs(speed), 0, 255));
}

void motorB(int speed) {
  digitalWrite(BIN1, speed >= 0 ? HIGH : LOW);
  digitalWrite(BIN2, speed >= 0 ? LOW  : HIGH);
  analogWrite(PWMB, constrain(abs(speed), 0, 255));
}

void setup() {
  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT); pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT); pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);  // enable driver
}

void loop() {
  motorA(200);  motorB(200);   // both forward
  delay(2000);
  motorA(0);    motorB(0);      // stop
  delay(500);
  motorA(-200); motorB(-200);   // both reverse
  delay(2000);
  motorA(0);    motorB(0);
  delay(500);
}
