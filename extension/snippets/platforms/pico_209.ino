// snippet: pico_209
// tags: pico, rp2040, raspberry-pi, servo
// boards: rp2040
// NOTE: pins 0/1 are hardware Serial RX/TX on AVR — safe on RP2040 only.
// library: Servo
// description: RPi Pico: dual servo on pins 0 and 1
// prompts: dual servo Pico, two servos RP2040, servo pair Raspberry Pi Pico, control two servos Pi Pico, servo1 servo2 RP2040, multi servo Pico
// verified: compile-only
// source: kd-generated (id 209)
// exact-include: Servo.h

#include <Servo.h>

Servo servo1;
Servo servo2;

void setup() {
  Serial.begin(115200);
  servo1.attach(0);
  servo2.attach(1);
  servo1.write(90);
  servo2.write(90);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.startsWith("SERVO1")) {
      int angle = command.substring(8).toInt();
      servo1.write(angle);
    } else if (command.startsWith("SERVO2")) {
      int angle = command.substring(8).toInt();
      servo2.write(angle);
    } else if (command == "CENTER") {
      servo1.write(90);
      servo2.write(90);
    }
  }
}
