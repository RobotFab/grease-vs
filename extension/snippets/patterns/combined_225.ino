// snippet: combined_225
// tags: combined, multi-sensor, millis, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Ultrasonic servo scanner: HC-SR04 pins 7/8, servo pin 9
// prompts: ultrasonic, servo, scanner, hc-sr04, servo, command
// verified: compile-only
// source: kd-generated (id 225)
// exact-include: Servo.h

#include <Servo.h>

#define TRIG_PIN  7
#define ECHO_PIN  8
#define SERVO_PIN 9

Servo scanServo;
int servoPos = 0;
int servoDir = 1;
unsigned long prevMs = 0;
const unsigned long STEP_MS = 100;  // 100ms per step

void setup() {
  Serial.begin(115200);
  scanServo.attach(SERVO_PIN);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= STEP_MS) {
    prevMs = now;
    scanServo.write(servoPos);

    // Trigger HC-SR04: 10µs pulse on TRIG, read echo on ECHO
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // 30ms timeout
    int dist = duration / 58;

    Serial.print("ANG:");
    Serial.print(servoPos);
    Serial.print(":DIST:");
    Serial.println(dist);

    servoPos += servoDir;
    if (servoPos >= 180 || servoPos <= 0) servoDir = -servoDir;
  }
}
