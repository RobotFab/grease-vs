// snippet: motor_brushless_274
// tags: motor, brushless, esc, pwm, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: ESC controlled by potentiometer on A0
// prompts: controlled, potentiometer, include, servo, declare, servo
// verified: compile-only
// source: kd-generated (id 274)
// exact-include: Servo.h

#include <Servo.h>

Servo esc;

void armESC() {
  esc.writeMicroseconds(1000);  // minimum throttle
  delay(2000);                  // wait for ESC to arm (beep sequence)
}

void setup() {
  Serial.begin(9600);
  esc.attach(9, 1000, 2000);  // min 1000µs, max 2000µs
  armESC();
  Serial.println("ESC armed");
}

void loop() {
  unsigned long now = millis();

  static unsigned long prevThrottleMs = 0;
  if (now - prevThrottleMs >= 50) {  // 20Hz update rate
    prevThrottleMs = now;
    int potValue = analogRead(A0);
    int throttle = map(potValue, 0, 1023, 1000, 2000);
    esc.writeMicroseconds(throttle);  // servo pulse width in us (1000=0deg, 1500=90deg, 2000=180deg)
    Serial.print("Pot value: ");
    Serial.print(potValue);
    Serial.print("  Throttle: ");
    Serial.println(throttle);
  }

  static unsigned long prevPrintMs = 0;
  if (now - prevPrintMs >= 100) {  // Print every 100ms
    prevPrintMs = now;
    Serial.print("Current throttle: ");
    Serial.println(esc.read());
  }
}
