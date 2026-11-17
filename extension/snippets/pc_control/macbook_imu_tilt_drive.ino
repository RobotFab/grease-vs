// snippet: macbook_imu_tilt_drive
// tags: macbook, apple-silicon, imu, accelerometer, tilt, serial, pc, robot, drive, python
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Drive a robot by tilting a MacBook (M1/M2/M3/M4). PC reads the built-in Bosch
//   BMI286 via IOKit HID and streams "T:<throttle> S:<steer>\n" packets over serial.
//   throttle and steer are integers -100..100.
// prompts: MacBook tilt drive, MacBook accelerometer robot, M1 M2 tilt control, Apple Silicon IMU serial, tilt to drive MacBook
//
// ---- Python companion (macOS only, Apple Silicon) ----
//   pip install pyserial
//   pip install apple-silicon-accelerometer   # olvvier/apple-silicon-accelerometer on GitHub
//   # requires sudo / run as root for IOKit HID access
//
//   import serial, time
//   from apple_accelerometer import Accelerometer
//
//   ser = serial.Serial('/dev/tty.usbserial-*', 115200)  # match your port
//   acc = Accelerometer()
//   DEAD = 0.15    # deadzone in g
//   SCALE = 100.0  # g → -100..100
//
//   print("Tilt MacBook to drive. Ctrl-C to quit.")
//   try:
//       while True:
//           ax, ay, az = acc.read()   # g, g, g
//           # ax: tilt left/right (steer), ay: tilt forward/back (throttle)
//           throttle = int(max(-100, min(100, -ay * SCALE)))
//           steer    = int(max(-100, min(100,  ax * SCALE)))
//           if abs(ay) < DEAD: throttle = 0
//           if abs(ax) < DEAD: steer    = 0
//           ser.write(f"T:{throttle} S:{steer}\n".encode())
//           time.sleep(0.05)
//   except KeyboardInterrupt:
//       ser.write(b"T:0 S:0\n")
//       ser.close()
// -----------------------------------------------------
//
// Repo: https://github.com/olvvier/apple-silicon-accelerometer

// ESP32-safe pins: GPIO 6/7/8/11 are flash-reserved and must never be used.
const int ENA = 5, IN1 = 14, IN2 = 15;
const int ENB = 12, IN3 = 4, IN4 = 3;
String inputLine = "";

void setMotors(int l, int r) {
  l = constrain(l, -255, 255);
  r = constrain(r, -255, 255);
  digitalWrite(IN1, l >= 0); digitalWrite(IN2, l < 0); analogWrite(ENA, abs(l));
  digitalWrite(IN3, r >= 0); digitalWrite(IN4, r < 0); analogWrite(ENB, abs(r));
}

// throttle + steer → differential left/right speeds
void drive(int throttle, int steer) {
  int left  = map(throttle - steer, -100, 100, -255, 255);
  int right = map(throttle + steer, -100, 100, -255, 255);
  setMotors(left, right);
}

void setup() {
  Serial.begin(115200);
  int pins[] = {ENA, IN1, IN2, ENB, IN3, IN4};
  for (int p : pins) pinMode(p, OUTPUT);
  Serial.println("MacBook IMU control ready");
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
    } else if (c != '\r') {
      inputLine += c;
    }
  }
}
