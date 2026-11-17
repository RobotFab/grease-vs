// snippet: misc_154
// tags: misc, millis, nonblocking, fsm, state-machine, enum, switch, timed
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: The SHAPE of a non-blocking finite state machine: a scoped `enum class` state,
//   a `switch (state)` in loop(), an enterState() that runs entry actions once, and an
//   entered-at timestamp so nothing ever calls delay(). This particular machine is a TIMED
//   sequence — each state ends when its own dwell time elapses — which is the right pattern
//   for a blink/cycle/pacing job and the WRONG one for a robot.
//   A robot's transitions are EVENTS and MEASUREMENTS, not clocks:
//     - "drive 40 cm"  → exit on an encoder COUNT target (romi32u4_fsm_route_drive)
//     - "next stop"    → exit on getSingleDebouncedPress() (romi_fsm_irq_button)
//     - "object found" → exit on a distance reading (romi_fsm_manipulator_scan_grab)
//   NOT: a millis() timeout standing in for a distance. Battery level, wheel slip and floor
//   friction all change how far a fixed time carries the robot; a count does not lie.
//   NOT: Serial.print() on every pass of loop(). loop() runs thousands of times a second, so
//   an unguarded print floods the link and becomes the slowest thing in the machine. Print on
//   TRANSITION, which is the only moment anything actually changed.
// prompts: finite state machine, enum State IDLE MEASURING REPORTING, non-blocking state machine, switch state millis, timed state sequence
// verified: compile-only
// source: kd-generated (id 154)

// `: uint8_t` keeps the state one byte — an atomic read/write on AVR, which matters the moment
// an ISR writes it. `enum class` stops the labels leaking into the global namespace (IDLE is a
// common name); use State::IDLE at the call sites.
enum class State : uint8_t { IDLE, MEASURING, REPORTING };

State state = State::IDLE;
unsigned long stateStartMs = 0;

const unsigned long IDLE_TIME      = 2000;
const unsigned long MEASURING_TIME = 3000;
const unsigned long REPORTING_TIME = 1000;

// Entry actions belong here, not in loop(): they run exactly once per transition, so the
// per-state code in loop() stays a pure "is it time to leave yet?" test.
void enterState(State s) {
  state = s;
  stateStartMs = millis();
  switch (s) {
    case State::IDLE:      Serial.println("state: IDLE");      break;
    case State::MEASURING: Serial.println("state: MEASURING"); break;
    case State::REPORTING: Serial.println("state: REPORTING"); break;
  }
}

void setup() {
  Serial.begin(9600);
  enterState(State::IDLE);
}

void loop() {
  unsigned long elapsed = millis() - stateStartMs;

  switch (state) {
    case State::IDLE:
      if (elapsed >= IDLE_TIME) enterState(State::MEASURING);
      break;

    case State::MEASURING:
      // A real machine takes its reading here. Replace the dwell test with the condition that
      // actually ends the state — a sample count, an encoder target, a button press.
      if (elapsed >= MEASURING_TIME) enterState(State::REPORTING);
      break;

    case State::REPORTING:
      if (elapsed >= REPORTING_TIME) enterState(State::IDLE);
      break;
  }
}
