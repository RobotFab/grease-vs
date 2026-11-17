// snippet: motor_brushless_278
// tags: motor, brushless, esc, pwm, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: ESC + servo mix for RC plane: Include Servo
// prompts: servo, plane, include, servo, declare, servo
// verified: compile-only
// source: kd-generated (id 278)
// exact-include: Servo.h

#include <Servo.h>

Servo throttleEsc;
Servo elevator;
int throttlePct = 0;
int elevAngle = 90;
const int ESC_PIN = 9;
const int ELEVATOR_PIN = 10;
const unsigned long ARM_TIME = 2000;  // 2 seconds

void setup() {
  throttleEsc.attach(ESC_PIN, 1000, 2000);  // min 1000µs, max 2000µs
  elevator.attach(ELEVATOR_PIN);
  Serial.begin(9600);

  // Arm ESC
  throttleEsc.writeMicroseconds(1000);  // servo pulse width in us (1000=0deg, 1500=90deg, 2000=180deg)
  delay(ARM_TIME);
  throttleEsc.writeMicroseconds(1500);  // neutral position
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs >= 100) {  // 10Hz update rate
    prevMs = now;

    // Read throttle and elevator values from serial
    if (Serial.available() > 0) {
      String command = Serial.readStringUntil('\n');
      int commaIndex = command.indexOf(',');
      if (commaIndex != -1) {
        throttlePct = command.substring(0, commaIndex).toInt();
        elevAngle = command.substring(commaIndex + 1).toInt();
      }
    }

    // Clamp values
    throttlePct = constrain(throttlePct, 0, 100);
    elevAngle = constrain(elevAngle, 0, 180);

    // Map throttle percentage to microseconds
    int throttleUs = map(throttlePct, 0, 100, 1000, 2000);

    // Write values to ESC and elevator
    throttleEsc.writeMicroseconds(throttleUs);  // servo pulse width in us (1000=0deg, 1500=90deg, 2000=180deg)
    elevator.write(elevAngle);

    // Print values
    Serial.print("THROTTLE: "); Serial.println(throttlePct);
    Serial.print("ELEV: "); Serial.println(elevAngle);
  }
}
