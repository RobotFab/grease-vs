// snippet: macbook_lid_angle_servo
// tags: macbook, lid, angle, servo, apple-silicon, imu, taigrr, iokit, hid, python
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: Servo
// description: Map MacBook lid open/close angle to a servo position. The taigrr/apple-silicon-
//   accelerometer Go library exposes the lid angle sensor (ALS-based) alongside accel/gyro.
//   Python reads it via the compiled Go binary and streams "A:<angle_deg>\n" over serial.
// prompts: MacBook lid angle servo, laptop lid position sensor, lid open close servo, IOKit HID sensor, Apple lid angle
//
// ---- Setup ----
//   1. Install Go: https://go.dev
//   2. go install github.com/taigrr/apple-silicon-accelerometer/cmd/accel-logger@latest
//      (or build from source: github.com/taigrr/apple-silicon-accelerometer)
//
// ---- Python companion ----
//   pip install pyserial
//
//   import subprocess, serial, re, time
//
//   ser = serial.Serial('/dev/tty.usbserial-*', 115200)
//   # taigrr's accel-logger outputs lines like:
//   #   {"lid_angle":72.3,"accel":{"x":0.01,"y":-0.98,"z":0.14}}
//   proc = subprocess.Popen(
//       ['accel-logger'],
//       stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True
//   )
//   import json
//   for line in proc.stdout:
//       try:
//           data = json.loads(line)
//           angle = data.get('lid_angle', 90)   # 0=closed, ~135=fully open
//           angle = max(0, min(135, angle))
//           # map lid angle 0-135° → servo 0-180°
//           servo_pos = int(angle * 180 / 135)
//           ser.write(f"A:{servo_pos}\n".encode())
//           time.sleep(0.05)
//       except: pass
// -----------------------------------------------
//
// Repo: https://github.com/taigrr/apple-silicon-accelerometer
// exact-include: Servo.h

#include <Servo.h>

Servo myServo;
String inputLine = "";

void setup() {
  Serial.begin(115200);
  myServo.attach(9);
  myServo.write(90);
  Serial.println("Lid angle servo ready");
}

void loop() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n') {
      int idx = inputLine.indexOf("A:");
      if (idx >= 0) {
        int angle = inputLine.substring(idx + 2).toInt();
        myServo.write(constrain(angle, 0, 180));
        Serial.print("servo: "); Serial.println(angle);
      }
      inputLine = "";
    } else if (c != '\r') {
      inputLine += c;
    }
  }
}
