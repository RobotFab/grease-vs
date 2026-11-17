// snippet: wasd_keyboard_drive
// tags: keyboard, wasd, serial, pc, robot, drive, motor, terminal, control, interactive
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: none
// description: Drive a differential-drive robot with WASD keys sent over serial from the PC.
//   w=forward  s=backward  a=turn-left  d=turn-right  space=stop
//   Deadman: motion stops on its own if no key arrives for 500 ms, so holding a key drives
//   (terminal auto-repeat) and letting go coasts to a stop. A teleop link that can drop
//   MUST have this — otherwise a closed terminal leaves the robot running away.
// prompts: WASD keyboard drive, keyboard robot control, W A S D motor serial, keyboard serial drive, terminal robot control
//
// ---- Python companion (run in a terminal on the PC) ----
//   pip install readchar pyserial
//
//   import serial, readchar
//   ser = serial.Serial('/dev/ttyUSB0', 115200)   # Windows: 'COM3'
//   print("WASD to drive, space to stop, q to quit")
//   while True:
//       k = readchar.readkey()
//       if k == 'q': break
//       if k in ('w','a','s','d',' '): ser.write(k.encode())
//   ser.close()
// --------------------------------------------------------

// ESP32-safe pins: GPIO 6/7/8/11 are flash-reserved and must never be used.
// The two ENx pins MUST be hardware-PWM pins on the target board, or the motor runs
// flat out at full voltage with no speed control. On Uno/Nano/Leonardo the PWM pins are
// 3, 5, 6, 9, 10, 11 (+13 on Leonardo) — 12 is NOT one of them. On ESP32 any GPIO works.
const int ENA = 5,  IN1 = 14, IN2 = 15;  // left motor  — 5 is PWM everywhere
const int ENB = 11, IN3 = 4,  IN4 = 3;   // right motor — 11 is PWM everywhere
const int SPEED = 180;
const int TURN_SPEED = 140;

// Teleop failsafe: stop if no command arrives for this long. Without it, a dropped USB
// cable or a closed terminal leaves the robot driving at full speed with nobody steering.
const unsigned long COMMAND_TIMEOUT_MS = 500;
unsigned long lastCommandMs = 0;
bool moving = false;

void setMotors(int l, int r) {
  l = constrain(l, -255, 255);
  r = constrain(r, -255, 255);
  digitalWrite(IN1, l >= 0); digitalWrite(IN2, l < 0); analogWrite(ENA, abs(l));
  digitalWrite(IN3, r >= 0); digitalWrite(IN4, r < 0); analogWrite(ENB, abs(r));
}

void setup() {
  Serial.begin(115200);
  int pins[] = {ENA, IN1, IN2, ENB, IN3, IN4};
  for (int p : pins) pinMode(p, OUTPUT);
  Serial.println("WASD ready");
}

void loop() {
  // Deadman check runs every pass, whether or not a key arrived.
  if (moving && millis() - lastCommandMs > COMMAND_TIMEOUT_MS) {
    setMotors(0, 0);
    moving = false;
    Serial.println("timeout — stopped");
  }

  if (!Serial.available()) return;
  char cmd = Serial.read();
  lastCommandMs = millis();

  switch (cmd) {
    case 'w': setMotors( SPEED,  SPEED);           moving = true;  Serial.println("fwd");   break;
    case 's': setMotors(-SPEED, -SPEED);           moving = true;  Serial.println("back");  break;
    case 'a': setMotors(-TURN_SPEED,  TURN_SPEED); moving = true;  Serial.println("left");  break;
    case 'd': setMotors( TURN_SPEED, -TURN_SPEED); moving = true;  Serial.println("right"); break;
    case ' ': setMotors(0, 0);                     moving = false; Serial.println("stop");  break;
  }
}
