// snippet: serial_report_96
// tags: serial, report, uart, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: On SWEEP command, sweep servo on pin 9 from 0 to 180 over 3 seconds using millis, report angle ev…
// prompts: sweep, command, sweep, servo, seconds, report
// verified: compile-only
// source: kd-generated (id 96)
// exact-include: Servo.h

#include <Servo.h>

Servo myServo;
int sweepAngle = 0;
bool sweeping = false;

void setup() {
  Serial.begin(115200);
  myServo.attach(9);
  myServo.write(0);
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();  // trim CRLF — without this, "SWEEP\r\n" never matches "SWEEP"
    if (command == "SWEEP") {
      sweeping = true;
    }
  }

  if (sweeping) {
    static unsigned long t = 0;
    if (millis() - t < 20) return;  // 50 Hz
    t = millis();

    sweepAngle += 1;
    if (sweepAngle > 180) {
      sweeping = false;
      sweepAngle = 0;
    }
    myServo.write(sweepAngle);

    if (sweepAngle % 15 == 0) {
      Serial.print("ANG:"); Serial.println(sweepAngle);
    }
  }
}
