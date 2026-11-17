// snippet: romi_fsm_serial_command
// tags: romi, romi32u4, pololu, fsm, state-machine, serial, serial-read, keyboard, command, uart, motor, drive, lcd, leonardo
// boards: romi32u4
// library: Romi32U4
// description: Serial-driven FSM on Romi 32U4 (detected as Arduino Leonardo). Single-character
//   commands typed over the USB serial monitor transition the state machine; the current state
//   is shown on the LCD and echoed back over serial. Motor output is a pure function of state.
//   Commands: 'F' forward, 'B' back, 'L' spin left, 'R' spin right, 'S' stop.
//   Pattern: read ONE char per available byte with Serial.read() (non-blocking) and switch on it
//     to set the next state. This is a true FSM -- the state persists and drives the motors every
//     loop -- not fire-and-forget command dispatch.
//   NOT: Serial.parseInt()/readString() with a blocking timeout in the drive loop -- it stalls
//     motor updates. Read available bytes and act on state instead.
//   NOTE: on the 32U4 'Serial' is the USB CDC port; wait is not required, but bytes only arrive
//     once the monitor is open.
// prompts: Romi serial command FSM, Romi keyboard control state machine, Romi Serial.read drive, Romi FSM serial input, Romi32U4 serial motor commands, Romi type command move robot, Romi single char command FSM, Romi drive forward back left right serial, Romi state machine keyboard, Romi serial controlled robot LCD
// not-tags: esp32, nano33iot, pico, softwareserial, bluetooth
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors motors;
Romi32U4LCD    lcd;

const int16_t DRIVE_SPEED = 150;  // forward/back wheel speed
const int16_t TURN_SPEED  = 120;  // spin speed (per wheel, opposed)

// FSM states — each maps to a fixed motor command
enum State : uint8_t { STOPPED, FORWARD, BACKWARD, LEFT, RIGHT };
State state    = STOPPED;
State shownState = FORWARD;  // != state so the first frame refreshes the LCD

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

void showState() {
  lcd.clear();
  lcd.gotoXY(0, 0);
  lcd.print("State:");
  lcd.gotoXY(0, 1);
  lcd.print(stateName(state));
}

void setup() {
  Serial.begin(9600);
  Serial.println("Commands: F B L R S");
}

void loop() {
  // ---- Serial input: one command character transitions the FSM ----
  while (Serial.available()) {
    char c = Serial.read();
    switch (c) {
      case 'F': case 'f': state = FORWARD;  break;
      case 'B': case 'b': state = BACKWARD; break;
      case 'L': case 'l': state = LEFT;     break;
      case 'R': case 'r': state = RIGHT;    break;
      case 'S': case 's': state = STOPPED;  break;
      case '\r': case '\n': continue;  // ignore line endings, don't echo
      default:  continue;              // ignore anything else
    }
    Serial.print("-> "); Serial.println(stateName(state));
  }

  // ---- LCD reflects state changes only (avoids per-loop flicker) ----
  if (state != shownState) {
    showState();
    shownState = state;
  }

  // ---- Motor output is a pure function of the current state ----
  switch (state) {
    case STOPPED:  motors.setSpeeds(0, 0);                       break;
    case FORWARD:  motors.setSpeeds(DRIVE_SPEED, DRIVE_SPEED);   break;
    case BACKWARD: motors.setSpeeds(-DRIVE_SPEED, -DRIVE_SPEED); break;
    case LEFT:     motors.setSpeeds(-TURN_SPEED, TURN_SPEED);    break;
    case RIGHT:    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);    break;
  }
}
