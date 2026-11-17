// snippet: multi_motor_robot_590
// tags: robot, multi-motor, differential-drive, millis, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: RC car: rear drive motor via L298N (ENA=5,IN1=6,IN2=7) and front steering servo on pin 9
// prompts: drive, motor, l298n, (ena=5, in1=6, in2=7)
// verified: compile-only
// source: kd-generated (id 590)
// exact-include: Servo.h

#include <Servo.h>

const int ENA = 5, IN1 = 6, IN2 = 7;  // Motor pins
const int SERVO_PIN = 9;              // Servo pin
const int SERIAL_BAUD = 9600;          // Serial baud rate
const unsigned long UPDATE_INTERVAL = 50;  // 20 Hz update rate (50 ms)

Servo frontServo;

int currentSpeed = 0;
int currentAngle = 90;

void setup() {
  Serial.begin(SERIAL_BAUD);
  frontServo.attach(SERVO_PIN);
  frontServo.write(currentAngle);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  Serial.println("RC Car Controller. Use FWD <speed> and REV <speed> for motor, and ANG <angle> for steering.");
}

void driveMotor(int speed) {
  if (speed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else if (speed < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }
  // Clamp to the 8-bit PWM range: serial toInt() can hand us >255 (e.g. "FWD 1000"),
  // and analogWrite() only accepts 0-255 — an unclamped value gives a garbage duty.
  analogWrite(ENA, constrain(abs(speed), 0, 255));
}

void loop() {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();

  if (now - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = now;

    if (Serial.available() > 0) {
      String command = Serial.readStringUntil('\n');
      if (command.startsWith("FWD")) {
        currentSpeed = command.substring(4).toInt();
      } else if (command.startsWith("REV")) {
        currentSpeed = -command.substring(4).toInt();
      } else if (command.startsWith("ANG")) {
        currentAngle = command.substring(4).toInt();
        if (currentAngle < 45) currentAngle = 45;
        if (currentAngle > 135) currentAngle = 135;
      }
    }

    driveMotor(currentSpeed);
    frontServo.write(currentAngle);
  }
}
