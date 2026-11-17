// snippet: l298n_dc_motor
// tags: motor, l298n, dc, driver, pwm, h-bridge, speed, direction
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none (raw GPIO)
// description: Control two DC motors with L298N using raw digitalWrite and analogWrite
// prompts: L298N motor driver, DC motor control, H-bridge motor, forward backward motor, motor speed direction, two DC motors, motor PWM speed

// Motor A: IN1=7, IN2=8, ENA=9 (PWM)
// Motor B: IN3=5, IN4=4, ENB=6 (PWM)
const int IN1 = 7, IN2 = 8, ENA = 9;
const int IN3 = 5, IN4 = 4, ENB = 6;

void motorA(int speed) {
  // speed: -255 to 255
  if (speed > 0) {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    analogWrite(ENA, speed);
  } else if (speed < 0) {
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    analogWrite(ENA, -speed);
  } else {
    digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
    analogWrite(ENA, 0);
  }
}

void motorB(int speed) {
  if (speed > 0) {
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
    analogWrite(ENB, speed);
  } else if (speed < 0) {
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
    analogWrite(ENB, -speed);
  } else {
    digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
    analogWrite(ENB, 0);
  }
}

void setup() {
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);
}

void loop() {
  motorA(200);
  motorB(200);
  delay(2000);
  motorA(0);
  motorB(0);
  delay(1000);
}
