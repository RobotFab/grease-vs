// snippet: combined_124
// tags: combined, multi-sensor, millis, newping
// boards: avr, esp32, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: NewPing
// description: HC-SR04 controls motor speed: distance 0–50cm mapped to speed 0–255
// prompts: hc-sr04, controls, motor, speed, distance, 0–50cm
// verified: compile-only
// source: kd-generated (id 124)
// exact-include: NewPing.h

#include <NewPing.h>

#define TRIGGER 7
#define ECHO 6
#define MAX_CM 50
#define MOTOR_PIN 5
#define ENA_PIN 6

NewPing sonar(TRIGGER, ECHO, MAX_CM);
int motorSpeed = 0;
float distCm = 0.0;

unsigned long prevMs = 0;
const unsigned long INTERVAL = 50;

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < INTERVAL) return;
  prevMs = now;

  distCm = sonar.ping_cm();  // NewPing: distance in cm (blocking ~10us-4ms)
  if (distCm == 0) {
    Serial.println("out of range");
    motorSpeed = 0;
  } else {
    motorSpeed = map(distCm, 0, 50, 0, 255);
    if (distCm < 5) motorSpeed = -motorSpeed;
  }

  analogWrite(ENA_PIN, abs(motorSpeed));
  digitalWrite(MOTOR_PIN, motorSpeed > 0 ? HIGH : LOW);

  Serial.print("distance: "); Serial.print(distCm); Serial.println(" cm");
  Serial.print("motor speed: "); Serial.println(motorSpeed);
}
