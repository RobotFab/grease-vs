// snippet: sensor_distance_52
// tags: distance, hcsr04, ultrasonic, trigger, echo, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: HC-SR04 distance mapped to servo angle on pin 9
// prompts: hc-sr04, distance, mapped, servo, angle, declare
// verified: compile-only
// source: kd-generated (id 52)
// exact-include: Servo.h

#include <Servo.h>

const int TRIG_PIN = 7;
const int ECHO_PIN = 8;
const int SERVO_PIN = 9;
unsigned long prevMs = 0;
float distCm = 0.0;
Servo myServo;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(90);
}

long readCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // returns echo duration in us; divide by 58 for cm (speed of sound)
  return duration / 58;
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  distCm = readCm();
  if (distCm > 0) {
    int servoAngle = map(distCm, 0, 50, 0, 180);
    myServo.write(servoAngle);
    Serial.print("distance: ");
    Serial.print(distCm);
    Serial.print(" cm, servo angle: ");
    Serial.println(servoAngle);
  }
}
