// snippet: romi32u4_fsm_route_drive
// tags: romi, romi32u4, pololu, fsm, state-machine, encoder, odometry, dead reckoning, navigation, route, square, turn, differential-drive, wheels
// boards: romi32u4
// library: Romi32U4
// description: Drive a Romi 32U4 around a route with its own wheels under a finite state machine.
//   Route: forward 40 cm → turn right 90° → forward 40 cm → turn right 90° → ... → DONE.
//   Button A starts, button B aborts to IDLE from any state.
//   Every leg ends on an ENCODER COUNT target — never on a millis() timeout.
//
//   ENCODER MATH — derive it, do not hard-code a counts-per-cm literal:
//     Romi 32U4 with the 120:1 gearmotors → 12 counts/motor-rev × 120 = 1440 counts/wheel-rev.
//     Wheel diameter 70 mm, wheel track (centre-to-centre) 141 mm.
//     COUNTS_PER_CM = CPR / (PI * 7.0)             = 1440 / 21.99 = 65.5
//     A pivot turn of angle A sweeps each wheel through (A/360) * PI * TRACK,
//     so COUNTS_PER_DEG = (PI * TRACK_CM / 360) * COUNTS_PER_CM.
//
//   Romi32U4 API:
//     Romi32U4Motors motors    → motors.setSpeeds(left, right); clamps to ±300
//                                (motors.allowTurbo(true) raises the limit to ±400)
//     Romi32U4Encoders enc     → getCountsLeft/Right(), getCountsAndResetLeft/Right() → int16_t
//     Romi32U4ButtonA/B btn    → getSingleDebouncedPress() → bool (edge detect, self-debouncing)
//
//   NOT: delay() — every wait is an encoder-count comparison.
//   NOT: a millis() timeout standing in for distance. Time is not odometry; a timeout
//        gives a different distance on fresh batteries than on flat ones.
//   NOT: encoders.left() / encoders.read() — no such methods.
//   NOT: Servo.h — it takes Timer1, which Romi32U4Motors uses for wheel PWM.
// prompts: Romi drive square, Romi FSM route, Romi navigate waypoints, Romi state machine drive, Romi turn 90 degrees encoder, Romi dead reckoning FSM, Romi drive forward then turn, Romi32U4 wheels state machine, Romi multi leg route
// not-tags: esp32, nano33iot, pico
// exact-include: Romi32U4.h

#include <Romi32U4.h>

Romi32U4Motors   motors;
Romi32U4Encoders encoders;
Romi32U4ButtonA  btnA;
Romi32U4ButtonB  btnB;

// ── Geometry — every distance constant is derived, none is a magic number ──
const float CPR            = 1440.0f;  // counts per wheel revolution (120:1 gearmotors)
const float WHEEL_DIAM_CM  = 7.0f;     // 70 mm Romi wheel
const float TRACK_CM       = 14.1f;    // 141 mm between wheel centres

const float COUNTS_PER_CM  = CPR / (PI * WHEEL_DIAM_CM);              // ≈ 65.5
const float COUNTS_PER_DEG = (PI * TRACK_CM / 360.0f) * COUNTS_PER_CM; // ≈ 8.06

// ── Route ─────────────────────────────────────────────────────────────
const float LEG_CM      = 40.0f;
const float TURN_DEG    = 90.0f;
const uint8_t NUM_LEGS  = 4;    // 4 legs + 4 turns = a square

const int DRIVE_SPEED = 100;   // ±300
const int TURN_SPEED  = 80;    // slower: pivot turns are where odometry error creeps in

// ── FSM ───────────────────────────────────────────────────────────────
// Scoped enum: no bare IDLE/DONE leaking into the global namespace.
enum class State : uint8_t { IDLE, DRIVE, TURN, DONE };

State   state    = State::IDLE;
uint8_t legIndex = 0;
long    target   = 0;   // encoder-count target for the current leg

// ── Encoder helpers ───────────────────────────────────────────────────
// Zero both encoders at the start of every leg, then measure from zero.
void resetEncoders() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}

// Average magnitude of both wheels — works for forward, reverse and pivot turns,
// where the two wheels count in opposite directions.
long travelCounts() {
  long l = abs((long)encoders.getCountsLeft());
  long r = abs((long)encoders.getCountsRight());
  return (l + r) / 2;
}

// ── One entry point for every transition ──────────────────────────────
void enterState(State s) {
  state = s;
  switch (s) {
    case State::IDLE:
      motors.setSpeeds(0, 0);
      legIndex = 0;
      Serial.println("IDLE — press A to start");
      break;

    case State::DRIVE:
      resetEncoders();
      target = (long)(LEG_CM * COUNTS_PER_CM);
      motors.setSpeeds(DRIVE_SPEED, DRIVE_SPEED);
      Serial.print("DRIVE leg "); Serial.println(legIndex + 1);
      break;

    case State::TURN:
      resetEncoders();
      target = (long)(TURN_DEG * COUNTS_PER_DEG);
      // Pivot right: left wheel forward, right wheel reverse.
      motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
      Serial.println("TURN 90");
      break;

    case State::DONE:
      motors.setSpeeds(0, 0);
      Serial.println("DONE — press A to run again");
      break;
  }
}

void setup() {
  Serial.begin(9600);
  enterState(State::IDLE);
}

void loop() {
  // Abort from anywhere. Checked before the switch so it always wins.
  if (btnB.getSingleDebouncedPress() && state != State::IDLE) {
    enterState(State::IDLE);
    return;
  }

  switch (state) {
    case State::IDLE:
      if (btnA.getSingleDebouncedPress()) enterState(State::DRIVE);
      break;

    case State::DRIVE:
      if (travelCounts() >= target) {
        motors.setSpeeds(0, 0);
        enterState(State::TURN);
      }
      break;

    case State::TURN:
      if (travelCounts() >= target) {
        motors.setSpeeds(0, 0);
        legIndex++;
        if (legIndex >= NUM_LEGS) enterState(State::DONE);
        else                      enterState(State::DRIVE);
      }
      break;

    case State::DONE:
      if (btnA.getSingleDebouncedPress()) {
        legIndex = 0;   // restart the route, not just the last leg
        enterState(State::DRIVE);
      }
      break;
  }
}
