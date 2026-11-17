// snippet: gamepad_joystick_drive
// tags: gamepad, joystick, controller, xbox, ps4, pygame, serial, pc, robot, drive, analog
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Drive a robot with any USB gamepad/joystick (Xbox, PS4, PS5, generic).
//   Left stick Y = throttle, left stick X = steer. Sends "T:<n> S:<n>\n" packets.
// prompts: gamepad robot drive, USB joystick control, Xbox PS4 controller serial, pygame joystick robot, analog stick drive
//
// ---- Python companion ----
//   pip install pyserial pygame
//
//   import serial, pygame, time
//
//   pygame.init()
//   pygame.joystick.init()
//   if pygame.joystick.get_count() == 0:
//       raise RuntimeError("No gamepad found")
//
//   js  = pygame.joystick.Joystick(0)
//   js.init()
//   ser = serial.Serial('/dev/ttyUSB0', 115200)  # Windows: 'COM3'
//   DEAD = 0.08   # analog stick deadzone
//
//   print(f"Controller: {js.get_name()}")
//   print("Left stick to drive. Ctrl-C to quit.")
//
//   try:
//       while True:
//           pygame.event.pump()
//           ax = js.get_axis(0)   # left stick X  (-1=left, +1=right)
//           ay = js.get_axis(1)   # left stick Y  (-1=up,   +1=down)
//           if abs(ax) < DEAD: ax = 0
//           if abs(ay) < DEAD: ay = 0
//           throttle = int(-ay * 100)   # invert Y: push forward = positive throttle
//           steer    = int( ax * 100)
//           ser.write(f"T:{throttle} S:{steer}\n".encode())
//           time.sleep(0.05)
//   except KeyboardInterrupt:
//       ser.write(b"T:0 S:0\n")
//       ser.close()
//       pygame.quit()
// -------------------------
//
// Axis mapping varies by controller — print js.get_axis(i) for i in range(6) to identify yours.

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
  Serial.println("Gamepad control ready");
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
