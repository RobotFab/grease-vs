// snippet: drv8833_basic
// tags: motor, drv8833, dc, driver, dual, h-bridge, pwm, ti
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none (raw GPIO)
// description: Dual DC motor control with DRV8833 using raw PWM — no library needed
// prompts: DRV8833 motor driver, dual DC motor DRV8833, Texas Instruments H-bridge, two motor raw PWM, small DC motor

// Motor A: AIN1=9 (PWM), AIN2=8 (PWM)
// Motor B: BIN1=6 (PWM), BIN2=5 (PWM)
const int AIN1 = 9, AIN2 = 8;
const int BIN1 = 6, BIN2 = 5;

void motorA(int speed) {
  // speed: -255 to 255, both pins PWM
  if (speed >= 0) {
    analogWrite(AIN1, speed);
    analogWrite(AIN2, 0);
  } else {
    analogWrite(AIN1, 0);
    analogWrite(AIN2, -speed);
  }
}

void motorB(int speed) {
  if (speed >= 0) {
    analogWrite(BIN1, speed);
    analogWrite(BIN2, 0);
  } else {
    analogWrite(BIN1, 0);
    analogWrite(BIN2, -speed);
  }
}

void setup() {
  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
}

void loop() {
  motorA(180);
  motorB(180);
  delay(2000);
  motorA(0);
  motorB(0);
  delay(500);
}
