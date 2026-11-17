// snippet: motor_speed_pwm
// tags: motor, pwm, speed, analogwrite, dc, direction, enable, h-bridge
// boards: esp32
// library: none
// description: Control DC motor speed with PWM and direction with two digital pins (generic H-bridge)
// prompts: PWM motor speed control, DC motor direction speed, H-bridge PWM analogWrite, variable speed motor control

const int EN  = 5;   // PWM enable (ENA on L298N)
const int IN1 = 18;  // direction pin (ESP32: off flash-reserved GPIO 6-11)
const int IN2 = 19;  // direction pin

void motorForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(EN, constrain(speed, 0, 255));
}

void motorBackward(int speed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(EN, constrain(speed, 0, 255));
}

void motorStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(EN, 0);
}

void setup() {
  pinMode(EN, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
}

void loop() {
  motorForward(200); delay(2000);
  motorStop();       delay(500);
  motorBackward(150); delay(2000);
  motorStop();       delay(500);
}
