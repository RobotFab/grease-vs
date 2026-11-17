// snippet: motor_brushless_276
// tags: motor, brushless, esc, pwm, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: ESC with safe arming: Include Servo
// prompts: arming, include, servo, declare, servo, globally
// verified: compile-only
// source: kd-generated (id 276)
// exact-include: Servo.h

#include <Servo.h>

const int ESC_PIN = 9;
Servo esc;
bool armed = false;
int safePct = 0;

void setup() {
  Serial.begin(9600);
  esc.attach(ESC_PIN, 1000, 2000);  // min 1000µs, max 2000µs
  Serial.println("Ready. Commands: ARM, DISARM, THROTTLE <pct>");
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();

    if (armed) {
      if (line.startsWith("THROTTLE ")) {
        int pct = line.substring(9).toInt();
        pct = constrain(pct, 0, 100);
        setThrottle(pct);
      }
    } else {
      if (line == "ARM") {
        armESC();
      } else if (line == "DISARM") {
        disarmESC();
      }
    }
  }
}

void armESC() {
  esc.writeMicroseconds(1000);  // Minimum throttle
  delay(3000);                  // Wait for ESC to arm (beep sequence)
  armed = true;
  Serial.println("ESC armed");
}

void disarmESC() {
  esc.writeMicroseconds(1000);  // Minimum throttle
  armed = false;
  Serial.println("ESC disarmed");
}

void setThrottle(int pct) {
  int throttle = map(pct, 0, 100, 1000, 2000);
  esc.writeMicroseconds(throttle);  // servo pulse width in us (1000=0deg, 1500=90deg, 2000=180deg)
}
