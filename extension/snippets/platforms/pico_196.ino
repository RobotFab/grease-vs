// snippet: pico_196
// tags: pico, rp2040, raspberry-pi, servo
// boards: rp2040
// library: Servo
// description: RPi Pico: drive servo on pin 15 via Servo library
// prompts: servo Pico, control servo RP2040, servo Raspberry Pi Pico, sweep servo Pico, servo motor Pi Pico, Servo library RP2040
// verified: compile-only
// source: kd-generated (id 196)
// exact-include: Servo.h

#include <Servo.h>

Servo myServo;
int currentAngle = 90;

void setup() {
  Serial.begin(115200);
  // Pin 15 — on RP2040 all GPIO pins are safe for servo PWM.
  // Avoid pin 0/1 on AVR/megaavr (hardware Serial RX/TX conflict).
  myServo.attach(15);
  myServo.write(currentAngle);
}

void loop() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.startsWith("SERVO")) {
      int angle = cmd.substring(6).toInt();
      if (angle >= 0 && angle <= 180) {
        myServo.write(angle);
        currentAngle = angle;
        Serial.print("SERVO set to: "); Serial.println(angle);
      } else {
        Serial.println("Invalid angle. Use 0-180.");
      }
    }
  }
}
