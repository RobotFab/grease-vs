// snippet: decl_practice_140
// tags: globals, declare, millis, nonblocking, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Declare Servo servo1, servo2 at global scope
// prompts: declare, servo, servo1, servo2, scope, servo1
// verified: compile-only
// source: kd-generated (id 140)
// exact-include: Servo.h

#include <Servo.h>
#include <math.h>

Servo servo1, servo2;

void setup() {
  servo1.attach(9);
  servo2.attach(10);
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs >= 20) {  // 50Hz update rate
    prevMs = now;

    // Use float math for sin/cos, then map to servo angle
    // map() truncates floats — compute angle directly instead
    float t = now / 1000.0f;  // seconds since boot
    float s = sin(t);
    float c = cos(t);

    // Map -1..1 to 0..180 degrees using float arithmetic
    int angle1 = (int)((s + 1.0f) * 90.0f);
    int angle2 = (int)((c + 1.0f) * 90.0f);
    angle1 = constrain(angle1, 0, 180);
    angle2 = constrain(angle2, 0, 180);

    servo1.write(angle1);
    servo2.write(angle2);
  }
}
