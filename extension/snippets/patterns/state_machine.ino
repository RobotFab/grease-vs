// snippet: state_machine
// tags: state machine, fsm, state, enum, switch, logic, control flow, robot
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Enum-based finite state machine — the standard robot behavior pattern.
//   IDLE → (button press) → DRIVE → (obstacle) → BACKUP → TURN → DRIVE, with a
//   RECOVER state entered when BACKUP fails to clear the obstacle.
//   Every transition guard reads a real input: a debounced button or a sensor value.
//   Every state is reachable and every state has an exit.
//   enum class State : uint8_t → scoped (no global name collisions) and 1 byte,
//   so it is safe to write from an ISR on AVR without cli/sei.
//   NOT: enum State { IDLE, MOVING }; — unscoped members leak into the global
//        namespace and collide with library macros (IDLE, ERROR, MOVING are common).
//   NOT: if (false) / placeholder guards — they make states unreachable.
//   NOT: delay() — every wait is a millis() comparison against stateEnteredMs.
// prompts: state machine, finite state machine, FSM, robot behavior, multiple modes, switch states, enum states, mode switching logic

const uint8_t BUTTON_PIN = 2;   // to GND, INPUT_PULLUP → pressed reads LOW
const uint8_t SENSOR_PIN = A0;  // distance sensor: higher reading = closer

const int  OBSTACLE_THRESHOLD = 600;  // ADC counts — tune for your sensor
const unsigned long BACKUP_MS = 700;
const unsigned long TURN_MS   = 500;
const uint8_t MAX_BACKUP_TRIES = 3;

// Scoped enum: refer to members as State::IDLE, never bare IDLE.
enum class State : uint8_t { IDLE, DRIVE, BACKUP, TURN, RECOVER };

State state = State::IDLE;
unsigned long stateEnteredMs = 0;
uint8_t backupTries = 0;

// ── Debounced button edge detect (no delay) ──────────────────────────
bool lastButtonLow = false;
unsigned long lastEdgeMs = 0;

bool buttonPressed() {
  bool low = (digitalRead(BUTTON_PIN) == LOW);
  unsigned long now = millis();
  if (low != lastButtonLow && now - lastEdgeMs > 30) {  // 30 ms debounce
    lastEdgeMs = now;
    lastButtonLow = low;
    return low;  // report the press edge only
  }
  return false;
}

bool obstacleAhead() {
  return analogRead(SENSOR_PIN) > OBSTACLE_THRESHOLD;
}

// ── Motion primitives — replace with your motor driver ───────────────
void driveForward() { /* motors forward */ }
void driveReverse() { /* motors reverse */ }
void turnInPlace()  { /* motors opposite directions */ }
void stopMotors()   { /* motors off */ }

// ── One entry point for every transition ─────────────────────────────
// Assigns the state, timestamps it, and performs the entry action.
void enterState(State s) {
  state = s;
  stateEnteredMs = millis();
  switch (s) {
    case State::IDLE:    stopMotors();    Serial.println("IDLE");    break;
    case State::DRIVE:   driveForward();  Serial.println("DRIVE");   break;
    case State::BACKUP:  driveReverse();  Serial.println("BACKUP");  break;
    case State::TURN:    turnInPlace();   Serial.println("TURN");    break;
    case State::RECOVER: stopMotors();    Serial.println("RECOVER"); break;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  enterState(State::IDLE);
}

void loop() {
  unsigned long now = millis();
  unsigned long elapsed = now - stateEnteredMs;

  switch (state) {
    case State::IDLE:
      // Button starts the run.
      if (buttonPressed()) {
        backupTries = 0;
        enterState(State::DRIVE);
      }
      break;

    case State::DRIVE:
      // Sensor decides — this guard reads real hardware, never a constant.
      if (obstacleAhead()) enterState(State::BACKUP);
      else if (buttonPressed()) enterState(State::IDLE);  // press again to stop
      break;

    case State::BACKUP:
      if (elapsed >= BACKUP_MS) {
        backupTries++;
        // Still blocked after backing up? Give up and ask for help.
        if (obstacleAhead() && backupTries >= MAX_BACKUP_TRIES) {
          enterState(State::RECOVER);
        } else {
          enterState(State::TURN);
        }
      }
      break;

    case State::TURN:
      if (elapsed >= TURN_MS) {
        if (obstacleAhead()) enterState(State::BACKUP);  // still blocked, try again
        else                 enterState(State::DRIVE);   // clear — resume
      }
      break;

    case State::RECOVER:
      // Terminal until the operator intervenes: clear the path, then press.
      if (!obstacleAhead() && buttonPressed()) {
        backupTries = 0;
        enterState(State::DRIVE);
      }
      break;
  }
}
