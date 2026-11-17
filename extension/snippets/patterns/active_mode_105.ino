// snippet: active_mode_105
// tags: active, serial, command, response, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Active sketch: SCAN moves servo on pin 9 from 0 to 180 in 10 degree steps, measures HC-SR04 distance
// prompts: active, sketch, moves, servo, degree, steps
// verified: compile-only
// source: kd-generated (id 105)
// exact-include: Servo.h

#include <Servo.h>

const int SERVO_PIN = 9;
const int TRIG_PIN  = 7;  // separate from servo pin
const int ECHO_PIN  = 8;

bool scanning = false;
int scanAngle = 0;
Servo myServo;
float distCm = 0.0;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(0);
  scanning = true;
}

void loop() {
  static unsigned long prevMs = 0;
  const unsigned long STEP_MS = 50;  // 10 degrees every 50ms

  if (scanning) {
    unsigned long now = millis();
    if (now - prevMs >= STEP_MS) {
      prevMs = now;
      scanAngle += 10;
      if (scanAngle > 180) {
        scanning = false;
      } else {
        myServo.write(scanAngle);
        distCm = readDistance();
        Serial.print("DIST:");
        Serial.print(scanAngle);
        Serial.print(":");
        Serial.println(distCm);
      }
    }
  }
}

float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  // 30ms timeout (~5m max range) prevents blocking if no echo
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // returns echo duration in us; divide by 58 for cm (speed of sound)
  return duration / 58.0;
}
