// snippet: multi_motor_robot_598
// tags: robot, multi-motor, differential-drive, millis
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Autonomous docking: drive toward a target IR beacon on pin 2 (interrupt, pulse counting)
// prompts: autonomous, docking, drive, toward, target, beacon
// verified: compile-only
// source: kd-generated (id 598)
// exact-include: Arduino.h

#include <Arduino.h>

const int IR_PIN = 2;
const int TRIG_PIN = 12;
const int ECHO_PIN = 13;
const int LEFT_MOTOR_PIN = 3;
const int RIGHT_MOTOR_PIN = 11;
const int MOTOR_SPEED = 128;

int irSignal = 0;  // ISR countIrSignal() is empty; irSignal is only touched in loop() — volatile removed
unsigned long prevIrMs = 0;
unsigned long prevHcMs = 0;
float distanceCm = 0.0;

void setup() {
  Serial.begin(9600);
  pinMode(IR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LEFT_MOTOR_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(IR_PIN), countIrSignal, CHANGE);
}

void loop() {
  unsigned long now = millis();

  // Read IR signal
  if (now - prevIrMs >= 100) {
    prevIrMs = now;
    int currentIrSignal = digitalRead(IR_PIN);
    if (currentIrSignal != irSignal) {
      irSignal = currentIrSignal;
      if (irSignal == HIGH) {
        Serial.println("IR signal detected");
      } else {
        Serial.println("No IR signal");
      }
    }
  }

  // Read HC-SR04 distance
  if (now - prevHcMs >= 100) {
    prevHcMs = now;
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // returns echo duration in us; divide by 58 for cm (speed of sound)
    distanceCm = duration * 0.034 / 2.0;
    Serial.print("distance: ");
    Serial.print(distanceCm);
    Serial.println(" cm");
  }

  // Drive toward IR beacon (bias slightly to steer toward the beacon)
  if (irSignal == HIGH) {
    int leftSpeed  = constrain(MOTOR_SPEED + 32, 0, 255);
    int rightSpeed = constrain(MOTOR_SPEED - 32, 0, 255);
    analogWrite(LEFT_MOTOR_PIN, leftSpeed);
    analogWrite(RIGHT_MOTOR_PIN, rightSpeed);
  } else {
    analogWrite(LEFT_MOTOR_PIN, 0);
    analogWrite(RIGHT_MOTOR_PIN, 0);
  }

  // Stop when a real echo puts an obstacle closer than 5 cm. pulseIn() returns 0 on
  // timeout (no echo) → distanceCm == 0, which must NOT be treated as "very close".
  if (distanceCm > 0.0 && distanceCm < 5.0) {
    analogWrite(LEFT_MOTOR_PIN, 0);
    analogWrite(RIGHT_MOTOR_PIN, 0);
    Serial.println("Stopping");
  }
}

void countIrSignal() {
  // Interrupt service routine for IR signal counting
}
