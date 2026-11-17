// snippet: motor_servo_17
// tags: servo, pwm, angle, sweep, millis
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Servo + buzzer coordination: servo on pin 9 sweeps 0–180
// prompts: servo, buzzer, coordination, servo, sweeps, 0–180
// verified: compile-only
// source: kd-generated (id 17)
// exact-include: Servo.h

#include <Servo.h>

Servo myServo;
int servoPos = 0;
int servoDir = 1;
const unsigned long STEP_MS = 10;  // one degree every 10ms = 1.8s full sweep
const unsigned long BUZZER_INTERVAL = 30 * STEP_MS;  // beep at each 30-degree position

unsigned long prevMs = 0;

void setup() {
  myServo.attach(9);
  pinMode(8, OUTPUT);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= STEP_MS) {
    prevMs = now;
    servoPos += servoDir;
    if (servoPos >= 180 || servoPos <= 0) servoDir = -servoDir;
    myServo.write(servoPos);

    if (servoPos % 30 == 0) {
      digitalWrite(8, HIGH);
      delay(50);  // beep duration
      digitalWrite(8, LOW);
    }
  }
}
