// snippet: multi_motor_robot_593
// tags: robot, multi-motor, differential-drive, millis, servo, newping
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo, NewPing
// description: Robot with gripper and drive: L298N differential drive (ENA=5,IN1=6,IN2=7; ENA=3,IN3=9,IN4=10)
// prompts: robot, gripper, drive, l298n, differential, drive
// verified: compile-only
// source: kd-generated (id 593)
// exact-include: Servo.h, NewPing.h

#include <Servo.h>
#include <NewPing.h>

// L298N enable pins carry the PWM speed signal and must be PWM-capable. On AVR that is
// 3/5/6/9/10/11 — pin 8 is NOT one, so a fixed analogWrite there latches full-on and the
// right track outruns the PWM'd left track (robot veers). Use PWM pin 3 for the right enable.
#define LEFT_ENA 5
#define LEFT_IN1 6
#define LEFT_IN2 7
#define RIGHT_ENA 3
#define RIGHT_IN3 9
#define RIGHT_IN4 10
#define SERVO_PIN 11
#define TRIG_PIN 12
#define ECHO_PIN 13
#define MAX_DIST 10

Servo gripper;
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DIST);

void setup() {
  Serial.begin(9600);
  gripper.attach(SERVO_PIN);
  gripper.write(0);  // Open gripper
  pinMode(LEFT_ENA, OUTPUT);
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(RIGHT_ENA, OUTPUT);
  pinMode(RIGHT_IN3, OUTPUT);
  pinMode(RIGHT_IN4, OUTPUT);
}

void loop() {
  int dist = sonar.ping_cm();  // NewPing: distance in cm (blocking ~10us-4ms)
  // NewPing returns 0 when there is no echo within range — treat that as "clear",
  // not as an obstacle sitting at 0 cm.
  if (dist != 0 && dist < MAX_DIST) {
    stopMotors();
    closeGripper();
    reverseMotors();
  } else {
    forwardMotors();
  }
}

void forwardMotors() {
  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);
  digitalWrite(RIGHT_IN3, HIGH);
  digitalWrite(RIGHT_IN4, LOW);
  analogWrite(LEFT_ENA, 200);
  analogWrite(RIGHT_ENA, 200);
}

void stopMotors() {
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);
  digitalWrite(RIGHT_IN3, LOW);
  digitalWrite(RIGHT_IN4, LOW);
}

void closeGripper() {
  gripper.write(90);  // Close gripper
  delay(1000);        // Wait for gripper to close
}

void reverseMotors() {
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);
  digitalWrite(RIGHT_IN3, LOW);
  digitalWrite(RIGHT_IN4, HIGH);
  analogWrite(LEFT_ENA, 200);
  analogWrite(RIGHT_ENA, 200);
}
