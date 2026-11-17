// snippet: romi32u4_serial1_command_fsm
// tags: romi, romi32u4, pololu, leonardo, fsm, state-machine, serial1, uart, esp32, esp32c6, bluetooth, ascii, wasd, command, remote, drive, motor, buzzer, led, level shift
// boards: romi32u4
// library: Romi32U4
// description: Romi 32U4 FSM driven by single ASCII bytes (W A S D, space to stop) arriving from
//   an ESP32-C6 on Serial1, with the USB Serial port still free for the computer. This is the
//   Romi half of the "ESP32 relays Bluetooth commands to the robot" pattern.
//
//   Serial1 IS NOT Serial. On the ATmega32U4 they are different peripherals:
//     Serial   = USB CDC, the cable to your computer. Costs nothing extra — the USB stack
//                is linked into every Leonardo sketch whether you use it or not.
//     Serial1  = the hardware UART on pins 0 (RX) and 1 (TX). Costs ~1000 bytes of flash.
//   Using both at once is the point: the ESP32 talks on Serial1 while you watch on Serial.
//   NOT SoftwareSerial — a real UART is right there, and SoftwareSerial disables interrupts
//     while it shifts bits, which corrupts the wheel encoder counts.
//
//   WIRING, AND THE ONE THING THAT DAMAGES HARDWARE:
//     ESP32-C6 TX (3.3 V) -> Romi RX (pin 0)   direct. The AVR's V_IH at 5 V is 3.0 V, so
//                                              3.3 V reads as HIGH with margin to spare.
//     Romi TX (pin 1, 5 V) -> ESP32-C6 RX      THROUGH A DIVIDER. The C6 is not 5 V tolerant.
//                                              1k series + 2k to ground gives 3.3 V.
//     Common ground between the two boards, always.
//   Cross the lines: TX to RX, RX to TX. Both boards at the same baud.
//
//   NOT I2C for this link. The Romi has to be the I2C MASTER for the PCA9685 (0x40), an AS5600
//     (0x36) and its onboard LSM6DS33 (0x6B) — and an AVR's TWI peripheral cannot be master and
//     slave at the same time. Serial1 adds a command channel without touching the bus.
//
//   The FSM is a true state machine: a byte changes the state, and the motors are written from
//   the state every loop. Not fire-and-forget dispatch inside the read.
// prompts: Romi receive commands from ESP32, Romi Serial1 WASD, Romi32U4 UART command FSM, Romi bluetooth relay commands, Romi ESP32 serial link, Romi drive from ESP32 bluetooth, Romi pin 0 pin 1 uart, Romi ASCII command state machine, Romi remote control serial, Romi level shift ESP32 5V 3.3V
// not-tags: esp32, nano33iot, pico, rp2040, softwareserial
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors   motors;
Romi32U4Buzzer   buzzer;
Romi32U4LCD      lcd;
// No begin() on any of these — Romi32U4 classes are ready on construction.

const int16_t DRIVE_SPEED = 150;
const int16_t TURN_SPEED  = 120;

// A remote link needs a deadman: if the ESP32 goes quiet, stop.
const unsigned long COMMAND_TIMEOUT_MS = 1000;
unsigned long lastCommandMs = 0;

enum State : uint8_t { STOPPED, FORWARD, BACKWARD, LEFT, RIGHT };
State state      = STOPPED;
State shownState = FORWARD;      // != state so the first frame draws

const char* stateName(State s) {
  switch (s) {
    case STOPPED:  return "STOP";
    case FORWARD:  return "FWD";
    case BACKWARD: return "BACK";
    case LEFT:     return "LEFT";
    case RIGHT:    return "RIGHT";
  }
  return "?";
}

void setup() {
  Serial.begin(9600);       // USB — the computer
  Serial1.begin(115200);    // pins 0/1 — the ESP32-C6
  lastCommandMs = millis();
  buzzer.play("!c32");      // ready
}

void loop() {
  unsigned long now = millis();

  // ── One byte per command, from the ESP32 ──────────────────────────
  while (Serial1.available()) {
    char c = Serial1.read();
    State next = state;
    switch (c) {
      case 'W': case 'w': next = FORWARD;  break;
      case 'S': case 's': next = BACKWARD; break;
      case 'A': case 'a': next = LEFT;     break;
      case 'D': case 'd': next = RIGHT;    break;
      case ' ':           next = STOPPED;  break;
      default: continue;               // ignore newlines and line noise
    }
    lastCommandMs = now;               // any valid byte feeds the deadman
    if (next != state) {
      state = next;
      buzzer.playFrequency(1200, 20, 10);   // non-blocking click
      Serial.println(stateName(state));     // mirror to the computer
    }
  }

  // ── Deadman: silence from the ESP32 means stop, not "keep going" ──
  if (state != STOPPED && now - lastCommandMs > COMMAND_TIMEOUT_MS) {
    state = STOPPED;
    Serial.println(F("link timeout"));
  }

  // ── LCD reflects state changes only, to avoid per-loop flicker ────
  if (state != shownState) {
    lcd.clear();
    lcd.gotoXY(0, 0);
    lcd.print(stateName(state));
    shownState = state;
  }

  // ── Motor output is a pure function of the current state ──────────
  switch (state) {
    case STOPPED:  motors.setSpeeds(0, 0);                       break;
    case FORWARD:  motors.setSpeeds(DRIVE_SPEED, DRIVE_SPEED);   break;
    case BACKWARD: motors.setSpeeds(-DRIVE_SPEED, -DRIVE_SPEED); break;
    case LEFT:     motors.setSpeeds(-TURN_SPEED, TURN_SPEED);    break;
    case RIGHT:    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);    break;
  }

  // ── Heartbeat back to the ESP32 at 2 Hz, one byte, no formatting ──
  static unsigned long beatMs = 0;
  if (now - beatMs >= 500) {
    beatMs = now;
    Serial1.write(state == STOPPED ? '.' : '>');
  }
}
