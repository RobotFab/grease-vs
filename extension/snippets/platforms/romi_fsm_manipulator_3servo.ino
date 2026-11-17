// snippet: romi_fsm_manipulator_3servo
// tags: romi, romi32u4, fsm, state-machine, pca9685, servo, manipulator, arm, gripper, pick, place, encoder, differential-drive
// boards: romi32u4
// library: Romi32U4, PCA9685
// description: Autonomous pick-and-place mission on Romi 32U4 with a 3-servo manipulator
//   (base rotate, arm lift, gripper) driven by a PCA9685 over I2C.
//   FSM: IDLE → DRIVE_FWD → PICKUP → DRIVE_BACK → DROP → IDLE.
//   Button A starts the mission; button B aborts and returns to IDLE.
//   Drive segments use encoder counts for distance; servo moves are non-blocking
//   (interpolated over time so the arm doesn't jerk).
//
//   Manipulator servo mapping (PCA9685 channels):
//     ch0 = BASE   (rotate 0–180°, 90 = center)
//     ch1 = ARM    (lift 0–180°, 0 = down, 180 = up)
//     ch2 = GRIPPER (0 = fully open, 180 = fully closed)
//
//   Romi32U4 API:
//     Romi32U4Motors motors → motors.setSpeeds(left, right); range −300..300
//       (the library clamps to ±300; call motors.allowTurbo(true) to raise it to ±400)
//     Romi32U4Encoders enc → getCountsAndResetLeft/Right() → int16_t
//     Romi32U4ButtonA/B → getSingleDebouncedPress() → bool (edge detect, self-debouncing)
//     Romi32U4LCD lcd → clear(); gotoXY(col,row); print("text"); 8×2 chars
//
//   PCA9685: I2C address 0x40 (default). Romi SDA=pin2, SCL=pin3.
//     setPWMFreq(50) = standard hobby servo (20 ms period).
//     writeMicroseconds(ch, us) sends pulse width directly — 1000 µs ≈ 0°, 2000 µs ≈ 180°.
//   WHY PCA9685 instead of Servo.h:
//     Servo.h on AVR captures Timer1 — the same timer Romi32U4Motors uses for drive PWM.
//     Running both causes erratic motor speed and servo jitter. PCA9685 generates PWM in
//     hardware over I2C, so no 32U4 timers are consumed.
// prompts: Romi manipulator FSM, Romi pick and place, Romi arm gripper PCA9685, Romi 3 servo robot arm, Romi state machine pick object, Romi32U4 servo manipulator, Romi gripper pick place autonomous, Romi PCA9685 arm gripper FSM
// not-tags: esp32, nano33iot, pico, mpu6050
// exact-include: Romi32U4.h, Wire.h, Adafruit_PWMServoDriver.h

#include <Romi32U4.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>  // PCA9685 library (Adafruit PWM Servo Driver Library)

// ── Hardware objects ──────────────────────────────────────────────
Romi32U4Motors   motors;
Romi32U4Encoders encoders;
Romi32U4ButtonA  btnA;
Romi32U4ButtonB  btnB;
Romi32U4LCD      lcd;

// PCA9685 at default I2C address 0x40.
// Romi32U4 I2C bus: SDA=pin2, SCL=pin3 — same bus as encoders/display, no conflict.
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// ── Servo channels & pulse limits ─────────────────────────────────
const uint8_t  CH_BASE    = 0;
const uint8_t  CH_ARM     = 1;
const uint8_t  CH_GRIPPER = 2;

const uint16_t SERVO_MIN_US = 1000;  // 1000 µs ≈ 0°
const uint16_t SERVO_MAX_US = 2000;  // 2000 µs ≈ 180°

// ── Pose presets (degrees) ────────────────────────────────────────
// Calibrate these for your physical arm geometry.
const int BASE_CENTER   = 90;   // base pointing straight ahead
const int BASE_PICK     = 90;   // base angle at pickup point (same as center here)
const int BASE_DROP     = 90;   // base angle at drop point

const int ARM_UP        = 160;  // arm raised (clear of object)
const int ARM_DOWN      = 20;   // arm lowered to object height
const int ARM_CARRY     = 120;  // mid-height while driving

const int GRIPPER_OPEN  = 0;    // fully open
const int GRIPPER_CLOSE = 130;  // fully closed (tune to your gripper)

// ── Drive constants ───────────────────────────────────────────────
const int  DRIVE_SPEED   = 120;   // motor speed (−300..300 default; ±400 only after allowTurbo(true))
const int  DRIVE_DIST_CM = 30;    // distance to drive forward to pickup point

// Derive the encoder constant — never hard-code a counts-per-cm literal.
// Romi 32U4 with the 120:1 gearmotors: 1440 counts per wheel revolution, 70 mm wheel.
//   counts/cm = CPR / (PI * wheel_diameter_cm) = 1440 / (PI * 7.0) = 65.5
const float WHEEL_DIAM_CM = 7.0f;   // 70 mm Romi wheel
const float CPR           = 1440.0f;  // counts per wheel revolution (120:1 gearmotors)
const float COUNTS_PER_CM = CPR / (PI * WHEEL_DIAM_CM);  // ≈ 65.5

// ── FSM ───────────────────────────────────────────────────────────
// enum State : uint8_t → 1-byte atomic write on AVR, safe even if read mid-cycle.
enum State : uint8_t {
  IDLE,
  DRIVE_FWD,   // drive forward to pickup point
  PICKUP,      // lower arm, close gripper, raise arm
  DRIVE_BACK,  // reverse to drop point
  DROP,        // lower arm, open gripper, raise arm
};

State state = IDLE;

// ── Non-blocking servo interpolation ──────────────────────────────
// Each servo channel tracks a current angle and a target angle.
// updateServos() moves current toward target at SERVO_DEG_PER_MS,
// so the arm moves smoothly instead of snapping.
struct ServoState {
  int current;       // current angle (degrees)
  int target;        // desired angle (degrees)
};
ServoState servos[3];  // indexed by channel 0..2

const float SERVO_DEG_PER_MS = 0.15f;  // ~1.2 s for a 180° sweep — tune for your servos
unsigned long servoMs = 0;

void setServoTarget(uint8_t ch, int degrees) {
  servos[ch].target = constrain(degrees, 0, 180);
}

void writeServo(uint8_t ch, int degrees) {
  uint16_t us = map(degrees, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
  pwm.writeMicroseconds(ch, us);  // writeMicroseconds handles 12-bit tick math internally
}

// Call every loop(); moves each servo toward its target by a small step.
void updateServos() {
  unsigned long now = millis();
  if (now - servoMs < 5) return;  // 200 Hz update rate
  float maxStep = (now - servoMs) * SERVO_DEG_PER_MS;
  servoMs = now;

  for (uint8_t i = 0; i < 3; i++) {
    int diff = servos[i].target - servos[i].current;
    if (diff == 0) continue;
    int step = (int)constrain(diff, -maxStep, maxStep);
    if (step == 0) step = (diff > 0) ? 1 : -1;  // ensure progress for small diffs
    servos[i].current += step;
    writeServo(i, servos[i].current);
  }
}

// Returns true when all 3 servos have reached their targets.
bool servosAtTarget() {
  return servos[0].current == servos[0].target &&
         servos[1].current == servos[1].target &&
         servos[2].current == servos[2].target;
}

// ── State helpers ─────────────────────────────────────────────────
void showState(const char* label) {
  lcd.clear();
  lcd.gotoXY(0, 0);
  lcd.print(label);
  Serial.println(label);
}

void allStop() {
  motors.setSpeeds(0, 0);
}

// Reset arm to a safe carry pose.
void armCarry() {
  setServoTarget(CH_BASE, BASE_CENTER);  // base pointing straight ahead while carrying
  setServoTarget(CH_ARM, ARM_CARRY);
  setServoTarget(CH_GRIPPER, GRIPPER_OPEN);
}

// ── Per-state timing ──────────────────────────────────────────────
unsigned long stateMs   = 0;  // entered-state timestamp
int           driveTarget = 0;  // encoder counts target for drive states

void enterState(State s) {
  state = s;
  stateMs = millis();
  switch (s) {
    case IDLE:
      allStop();
      armCarry();
      showState("IDLE");
      break;
    case DRIVE_FWD:
      encoders.getCountsAndResetLeft();   // zero both encoders
      encoders.getCountsAndResetRight();
      driveTarget = (int)(DRIVE_DIST_CM * COUNTS_PER_CM);
      motors.setSpeeds(DRIVE_SPEED, DRIVE_SPEED);
      showState("DRIVE FWD");
      break;
    case PICKUP:
      allStop();
      // Sequence the arm: lower → close → raise (done in loop via sub-timer)
      setServoTarget(CH_BASE, BASE_PICK);
      setServoTarget(CH_ARM, ARM_DOWN);
      setServoTarget(CH_GRIPPER, GRIPPER_OPEN);  // open first, then close after lowering
      showState("PICKUP");
      break;
    case DRIVE_BACK:
      encoders.getCountsAndResetLeft();
      encoders.getCountsAndResetRight();
      driveTarget = (int)(DRIVE_DIST_CM * COUNTS_PER_CM);  // same distance back
      motors.setSpeeds(-DRIVE_SPEED, -DRIVE_SPEED);
      showState("DRIVE BACK");
      break;
    case DROP:
      allStop();
      setServoTarget(CH_BASE, BASE_DROP);
      setServoTarget(CH_ARM, ARM_DOWN);
      // gripper opens after arm lowers (handled in loop)
      showState("DROP");
      break;
  }
}

// ── Pickup / drop sub-sequences (phased by time) ──────────────────
// PICKUP phases: 0 = lowering, 1 = closing gripper, 2 = raising
// DROP phases:   0 = lowering, 1 = opening gripper, 2 = raising
uint8_t subPhase = 0;

void updatePickup() {
  unsigned long elapsed = millis() - stateMs;
  switch (subPhase) {
    case 0:  // wait for arm to lower
      if (servosAtTarget()) {
        setServoTarget(CH_GRIPPER, GRIPPER_CLOSE);
        subPhase = 1;
        stateMs = millis();
      }
      break;
    case 1:  // wait for gripper to close
      if (servosAtTarget()) {
        setServoTarget(CH_ARM, ARM_UP);
        subPhase = 2;
        stateMs = millis();
      }
      break;
    case 2:  // wait for arm to raise → done
      if (servosAtTarget()) {
        enterState(DRIVE_BACK);
      }
      break;
  }
}

void updateDrop() {
  switch (subPhase) {
    case 0:  // wait for arm to lower
      if (servosAtTarget()) {
        setServoTarget(CH_GRIPPER, GRIPPER_OPEN);
        subPhase = 1;
        stateMs = millis();
      }
      break;
    case 1:  // wait for gripper to open
      if (servosAtTarget()) {
        setServoTarget(CH_ARM, ARM_UP);
        subPhase = 2;
        stateMs = millis();
      }
      break;
    case 2:  // wait for arm to raise → done
      if (servosAtTarget()) {
        enterState(IDLE);
      }
      break;
  }
}

// ── Setup ─────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  // PCA9685 init
  Wire.begin();
  pwm.begin();
  pwm.setPWMFreq(50);  // 50 Hz = standard hobby servo (20 ms period)
  // Do NOT set above ~330 Hz for analog servos — they overheat or stall.

  // Initialize servos to carry pose
  servos[0].current = servos[0].target = BASE_CENTER;
  servos[1].current = servos[1].target = ARM_UP;
  servos[2].current = servos[2].target = GRIPPER_OPEN;
  for (uint8_t i = 0; i < 3; i++) writeServo(i, servos[i].current);

  enterState(IDLE);
  Serial.println("Ready. Press A to start pick-and-place. B to abort.");
}

// ── Main loop ─────────────────────────────────────────────────────
void loop() {
  updateServos();  // always advance servo motion

  // Button B: abort → IDLE from anywhere
  if (btnB.getSingleDebouncedPress() && state != IDLE) {
    enterState(IDLE);
    return;
  }

  switch (state) {
    case IDLE:
      allStop();
      if (btnA.getSingleDebouncedPress()) {
        subPhase = 0;
        enterState(DRIVE_FWD);
      }
      break;

    case DRIVE_FWD: {
      int16_t dL = encoders.getCountsLeft();
      int16_t dR = encoders.getCountsRight();
      int dist = (abs(dL) + abs(dR)) / 2;
      if (dist >= driveTarget) {
        subPhase = 0;
        enterState(PICKUP);
      }
      break;
    }

    case PICKUP:
      updatePickup();
      break;

    case DRIVE_BACK: {
      int16_t dL = encoders.getCountsLeft();
      int16_t dR = encoders.getCountsRight();
      int dist = (abs(dL) + abs(dR)) / 2;
      if (dist >= driveTarget) {
        subPhase = 0;
        enterState(DROP);
      }
      break;
    }

    case DROP:
      updateDrop();
      break;
  }
}
