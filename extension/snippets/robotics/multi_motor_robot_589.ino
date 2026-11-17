// snippet: multi_motor_robot_589
// tags: robot, multi-motor, differential-drive, millis, tracked, l298n
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Tracked robot: two L298N modules controlling left track (ENA=5,IN1=6,IN2=7) and right track (ENA=11,IN3=9,IN4=10)
// prompts: tracked, robot, l298n, modules, controlling, track
// verified: compile-only
// source: kd-generated (id 589)
// exact-include: Arduino.h

#include <Arduino.h>

// L298N enable pins carry the PWM speed signal, so both must be PWM-capable. On classic
// AVR that is 3/5/6/9/10/11 — pin 8 is NOT a PWM pin, so the right track uses 11, not 8.
const int LEFT_ENA = 5, LEFT_IN1 = 6, LEFT_IN2 = 7;  // Left track pins
const int RIGHT_ENA = 11, RIGHT_IN3 = 9, RIGHT_IN4 = 10;  // Right track pins
const int SERIAL_BAUD = 9600;  // Serial baud rate
const unsigned long UPDATE_INTERVAL = 50;  // 20 Hz update rate (50 ms)

int leftSpeed = 0;
int rightSpeed = 0;

void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(LEFT_ENA, OUTPUT);
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(RIGHT_ENA, OUTPUT);
  pinMode(RIGHT_IN3, OUTPUT);
  pinMode(RIGHT_IN4, OUTPUT);
  Serial.println("Tracked Robot Controller. Use FWD <speed>, REV <speed>, LEFT <speed>, RIGHT <speed> for movement.");
}

void driveLeft(int speed) {
  if (speed > 0) {
    digitalWrite(LEFT_IN1, HIGH);
    digitalWrite(LEFT_IN2, LOW);
  } else if (speed < 0) {
    digitalWrite(LEFT_IN1, LOW);
    digitalWrite(LEFT_IN2, HIGH);
  } else {
    digitalWrite(LEFT_IN1, LOW);
    digitalWrite(LEFT_IN2, LOW);
  }
  // Clamp to the 8-bit PWM range: serial toInt() can hand us >255 (e.g. "FWD 1000"),
  // and analogWrite() only accepts 0-255 — an unclamped value gives a garbage duty.
  analogWrite(LEFT_ENA, constrain(abs(speed), 0, 255));
}

void driveRight(int speed) {
  if (speed > 0) {
    digitalWrite(RIGHT_IN3, HIGH);
    digitalWrite(RIGHT_IN4, LOW);
  } else if (speed < 0) {
    digitalWrite(RIGHT_IN3, LOW);
    digitalWrite(RIGHT_IN4, HIGH);
  } else {
    digitalWrite(RIGHT_IN3, LOW);
    digitalWrite(RIGHT_IN4, LOW);
  }
  analogWrite(RIGHT_ENA, constrain(abs(speed), 0, 255));
}

void loop() {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();

  if (now - lastUpdate >= UPDATE_INTERVAL) {
    lastUpdate = now;

    if (Serial.available() > 0) {
      String command = Serial.readStringUntil('\n');
      if (command.startsWith("FWD")) {
        leftSpeed = rightSpeed = command.substring(4).toInt();
      } else if (command.startsWith("REV")) {
        leftSpeed = rightSpeed = -command.substring(4).toInt();
      } else if (command.startsWith("LEFT")) {
        leftSpeed = -command.substring(5).toInt();
        rightSpeed = command.substring(5).toInt();
      } else if (command.startsWith("RIGHT")) {
        leftSpeed = command.substring(6).toInt();
        rightSpeed = -command.substring(6).toInt();
      }
    }

    driveLeft(leftSpeed);
    driveRight(rightSpeed);
  }
}
