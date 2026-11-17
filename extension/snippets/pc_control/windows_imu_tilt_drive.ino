// snippet: windows_imu_tilt_drive
// tags: windows, imu, accelerometer, tilt, serial, pc, robot, drive, winrt, yoga, thinkpad
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Drive a robot by tilting a Windows laptop that has a built-in accelerometer
//   (Lenovo Yoga/ThinkPad X1, Surface, many convertibles). Uses the WinRT Sensors API via
//   Python's winsdk package — no drivers needed, works on any Windows 10/11 machine
//   with an IIO accelerometer registered in Device Manager.
// prompts: Windows laptop tilt drive, WinRT accelerometer, Lenovo Yoga tilt control, Surface tilt robot, Windows IMU serial
//
// ---- Python companion (Windows only) ----
//   pip install pyserial winsdk
//
//   import asyncio, serial
//   from winsdk.windows.devices.sensors import Accelerometer
//
//   ser = serial.Serial('COM3', 115200)   # adjust port
//   DEAD  = 0.15
//   SCALE = 100.0
//
//   async def main():
//       acc = Accelerometer.get_default()
//       if acc is None:
//           print("No accelerometer found — check Device Manager")
//           return
//       acc.report_interval = 50   # ms
//       print("Tilt laptop to drive. Ctrl-C to quit.")
//       try:
//           while True:
//               r = acc.get_current_reading()
//               ax = r.acceleration_x   # g
//               ay = r.acceleration_y
//               throttle = int(max(-100, min(100, -ay * SCALE)))
//               steer    = int(max(-100, min(100,  ax * SCALE)))
//               if abs(ay) < DEAD: throttle = 0
//               if abs(ax) < DEAD: steer    = 0
//               ser.write(f"T:{throttle} S:{steer}\n".encode())
//               await asyncio.sleep(0.05)
//       except KeyboardInterrupt:
//           ser.write(b"T:0 S:0\n")
//           ser.close()
//
//   asyncio.run(main())
// ------------------------------------------
//
// Linux alternative (iio-sensor-proxy / sysfs):
//   cat /sys/bus/iio/devices/iio:device0/in_accel_x_raw
//   Scale factor in: in_accel_scale

// ESP32-safe pins: GPIO 6/7/8/11 are flash-reserved and must never be used.
const int ENA = 5, IN1 = 14, IN2 = 15;
const int ENB = 12, IN3 = 4, IN4 = 3;
String inputLine = "";

void setMotors(int l, int r) {
  l = constrain(l, -255, 255); r = constrain(r, -255, 255);
  digitalWrite(IN1, l >= 0); digitalWrite(IN2, l < 0); analogWrite(ENA, abs(l));
  digitalWrite(IN3, r >= 0); digitalWrite(IN4, r < 0); analogWrite(ENB, abs(r));
}

void drive(int throttle, int steer) {
  int left  = map(throttle - steer, -100, 100, -255, 255);
  int right = map(throttle + steer, -100, 100, -255, 255);
  setMotors(left, right);
}

void setup() {
  Serial.begin(115200);
  int pins[] = {ENA, IN1, IN2, ENB, IN3, IN4};
  for (int p : pins) pinMode(p, OUTPUT);
  Serial.println("Windows IMU control ready");
}

void loop() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n') {
      int tIdx = inputLine.indexOf("T:");
      int sIdx = inputLine.indexOf("S:");
      if (tIdx >= 0 && sIdx >= 0) {
        int throttle = inputLine.substring(tIdx + 2, sIdx - 1).toInt();
        int steer    = inputLine.substring(sIdx + 2).toInt();
        drive(throttle, steer);
      }
      inputLine = "";
    } else if (c != '\r') { inputLine += c; }
  }
}
