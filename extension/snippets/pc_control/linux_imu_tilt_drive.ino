// snippet: linux_imu_tilt_drive
// tags: linux, imu, accelerometer, iio, sysfs, tilt, serial, pc, robot, drive, thinkpad
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Drive a robot by tilting a Linux laptop with a built-in IIO accelerometer
//   (ThinkPad X1 Yoga, Dell XPS 2-in-1, many convertibles). Reads raw values from
//   /sys/bus/iio/devices/iio:device0/ — no extra packages needed beyond pyserial.
// prompts: Linux laptop tilt drive, ThinkPad accelerometer robot, IIO sysfs tilt control, Linux IMU serial robot, tilt to drive Linux
//
// ---- Python companion (Linux only) ----
//   pip install pyserial
//
//   import serial, time, os
//
//   IIO_PATH = "/sys/bus/iio/devices/iio:device0"   # adjust if needed
//   PORT     = "/dev/ttyUSB0"
//   DEAD     = 300     # raw units — adjust for your sensor
//   SCALE    = 0.008   # raw → g (read in_accel_scale for exact value)
//
//   def read_axis(axis):
//       with open(f"{IIO_PATH}/in_accel_{axis}_raw") as f:
//           return int(f.read().strip())
//
//   def find_iio():
//       """Find the first IIO device with acceleration axes."""
//       base = "/sys/bus/iio/devices"
//       for dev in sorted(os.listdir(base)):
//           if os.path.exists(f"{base}/{dev}/in_accel_x_raw"):
//               return f"{base}/{dev}"
//       return None
//
//   iio = find_iio()
//   if not iio: raise RuntimeError("No IIO accelerometer found")
//   IIO_PATH = iio
//   print(f"Using {iio}")
//
//   ser = serial.Serial(PORT, 115200)
//   try:
//       while True:
//           ax = read_axis('x') * SCALE
//           ay = read_axis('y') * SCALE
//           throttle = int(max(-100, min(100, -ay * 100)))
//           steer    = int(max(-100, min(100,  ax * 100)))
//           raw_x = read_axis('x')
//           raw_y = read_axis('y')
//           if abs(raw_x) < DEAD: steer    = 0
//           if abs(raw_y) < DEAD: throttle = 0
//           ser.write(f"T:{throttle} S:{steer}\n".encode())
//           time.sleep(0.05)
//   except KeyboardInterrupt:
//       ser.write(b"T:0 S:0\n")
//       ser.close()
// ----------------------------------------
//
// Find your IIO device:  ls /sys/bus/iio/devices/
// Check axes available:  ls /sys/bus/iio/devices/iio:device0/

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
  Serial.println("Linux IIO control ready");
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
