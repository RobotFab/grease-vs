// snippet: brushless_esc
// tags: motor, brushless, esc, servo, pwm, throttle, bldc, drone, rc
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Control brushless motor through ESC using Servo library — arming sequence required
// prompts: brushless motor ESC, drone motor control, ESC arming sequence, BLDC motor Arduino, throttle control ESC Servo
// exact-include: Servo.h

#include <Servo.h>

const int ESC_PIN = 9;
Servo esc;

void armESC() {
  esc.writeMicroseconds(1000);  // minimum throttle
  delay(3000);                  // wait for ESC to arm (beep sequence)
}

void setup() {
  Serial.begin(9600);
  esc.attach(ESC_PIN, 1000, 2000);  // min 1000µs, max 2000µs
  armESC();
  Serial.println("ESC armed");
}

void loop() {
  // ramp up
  for (int us = 1000; us <= 1500; us += 10) {
    esc.writeMicroseconds(us);  // servo pulse width in us (1000=0deg, 1500=90deg, 2000=180deg)
    delay(50);
  }
  delay(2000);
  // ramp down
  for (int us = 1500; us >= 1000; us -= 10) {
    esc.writeMicroseconds(us);  // servo pulse width in us (1000=0deg, 1500=90deg, 2000=180deg)
    delay(50);
  }
  delay(2000);
}
