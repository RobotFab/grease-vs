// snippet: romi_fsm_manipulator_scan_grab
// tags: romi, romi32u4, fsm, state-machine, pca9685, servo, manipulator, arm, gripper, scan, distance, hcsr04, ultrasonic, sensor, autonomous
// boards: romi32u4
// library: Romi32U4, PCA9685
// description: Sensor-triggered scan-and-grab on Romi 32U4 with a 3-servo manipulator
//   (base rotate, arm lift, gripper) driven by a PCA9685, and an HC-SR04 ultrasonic
//   distance sensor on the base for object detection.
//
//   FSM: IDLE → (btnA) → SCAN → (object found) → ALIGN → APPROACH → GRAB → RETREAT → IDLE.
//   The HC-SR04 rides on the BASE servo, so SCAN sweeps the sensor, not the chassis. That means
//   a detection carries an ANGLE as well as a distance, and the robot must turn to face the
//   object before it drives. Driving straight forward at an object found at 30° simply misses
//   it — record the base angle at detection and pivot by (angle − 90°) first. Any sketch that
//   detects with a steerable sensor and then drives on a fixed heading has this bug.
//   ALIGN and APPROACH both terminate on ENCODER COUNTS, never on a millis() timeout: a
//   flat battery or a carpet changes how far a fixed time carries the robot, a count does not.
//   Button B aborts at any time.
//
//   Manipulator servo mapping (PCA9685 channels):
//     ch0 = BASE    (rotate 0–180°, 90 = center)
//     ch1 = ARM     (lift 0–180°, 0 = down, 180 = up)
//     ch2 = GRIPPER (0 = fully open, 180 = fully closed)
//
//   HC-SR04 wiring (separate from PCA9685 — uses Romi GPIO, not I2C):
//     TRIG → Romi pin 8 (digital out)
//     ECHO → Romi pin 4 (digital in, via voltage divider if using 5V HC-SR04)
//   HC-SR04 is read RAW (10 µs trigger pulse, pulseIn() on echo, 58 µs/cm).
//   NOT: NewPing — NewPing and Romi32U4.h BOTH define the Timer4 overflow ISR
//     (__vector_41: NewPing timer methods vs PololuBuzzer) → link error
//     "multiple definition of __vector_41" on EVERY Romi sketch. Raw pulseIn() only.
//   readDistanceCm() returns distance in cm, or 0 if no echo (out of range) —
//     must check > 0 before comparing.
//
//   Romi32U4 API:
//     Romi32U4Motors motors → motors.setSpeeds(left, right); range −300..300
//       (library clamps to ±300; motors.allowTurbo(true) raises the limit to ±400)
//     Romi32U4Encoders encoders → getCountsLeft/Right(), getCountsAndResetLeft/Right() → int16_t
//       1440 counts per wheel revolution, 70 mm wheels, 141 mm track:
//       COUNTS_PER_CM = 1440 / (PI * 7.0) ≈ 65.5, COUNTS_PER_DEG = (PI * 14.1 / 360) * 65.5 ≈ 8.06
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
// prompts: Romi scan grab, Romi sensor manipulator, Romi ultrasonic arm, Romi PCA9685 scan grab, Romi FSM object detection, Romi32U4 servo arm gripper sensor, Romi autonomous grab ultrasonic, Romi HC-SR04 manipulator
// not-tags: esp32, nano33iot, pico, mpu6050, newping
// exact-include: Romi32U4.h, Wire.h, Adafruit_PWMServoDriver.h

#include <Romi32U4.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>  // PCA9685 library (Adafruit PWM Servo Driver Library)
// NOT: #include <NewPing.h> — Timer4 ISR conflict with Romi32U4's PololuBuzzer (link error)

// ── Hardware objects ──────────────────────────────────────────────
Romi32U4Motors   motors;
Romi32U4Encoders encoders;
Romi32U4ButtonA  btnA;
Romi32U4ButtonB  btnB;
Romi32U4LCD      lcd;

// ── Odometry constants (see header) ───────────────────────────────
const float CPR            = 1440.0f;   // counts per wheel revolution (120:1 gearmotors)
const float WHEEL_DIAM_CM  = 7.0f;      // 70 mm Romi wheel
const float TRACK_CM       = 14.1f;     // 141 mm between wheel centres
const float COUNTS_PER_CM  = CPR / (PI * WHEEL_DIAM_CM);               // ≈ 65.5
const float COUNTS_PER_DEG = (PI * TRACK_CM / 360.0f) * COUNTS_PER_CM; // ≈ 8.06

// PCA9685 at default I2C address 0x40.
// Romi32U4 I2C bus: SDA=pin2, SCL=pin3 — same bus as encoders/display, no conflict.
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// HC-SR04 ultrasonic sensor, read raw — NO NewPing (Timer4 ISR conflict with Romi32U4).
// TRIG on pin 8, ECHO on pin 4 — both free GPIO on Romi (not used by motors/encoders/I2C).
const uint8_t  TRIG_PIN = 8;
const uint8_t  ECHO_PIN = 4;
const uint16_t MAX_CM   = 200;  // echo timeout ceiling (prevents long blocking waits)

// ── Servo channels & pulse limits ─────────────────────────────────
const uint8_t  CH_BASE    = 0;
const uint8_t  CH_ARM     = 1;
const uint8_t  CH_GRIPPER = 2;

const uint16_t SERVO_MIN_US = 1000;  // 1000 µs ≈ 0°
const uint16_t SERVO_MAX_US = 2000;  // 2000 µs ≈ 180°

// ── Pose presets (degrees) ────────────────────────────────────────
// Calibrate these for your physical arm geometry.
const int BASE_SCAN_MIN  = 0;    // scan sweep start
const int BASE_SCAN_MAX  = 180;  // scan sweep end
const int BASE_CENTER    = 90;   // base pointing straight ahead

const int ARM_UP        = 160;  // arm raised (clear of object)
const int ARM_DOWN      = 20;   // arm lowered to object height
const int ARM_CARRY     = 120;  // mid-height while driving

const int GRIPPER_OPEN  = 0;    // fully open
const int GRIPPER_CLOSE = 130;  // fully closed (tune to your gripper)

// ── Detection & drive constants ───────────────────────────────────
const float GRAB_DIST_CM   = 8.0f;   // object within this distance → grab
const int   APPROACH_SPEED = 100;    // slow forward creep speed
const int   TURN_SPEED     = 100;    // pivot speed while aligning
const float APPROACH_CM    = 4.0f;   // how far to creep forward, in centimetres
const float RETREAT_CM     = 15.0f;  // how far to reverse after the grab

// ── FSM ───────────────────────────────────────────────────────────
// enum State : uint8_t → 1-byte atomic write on AVR, safe even if read mid-cycle.
enum State : uint8_t {
  IDLE,
  SCAN,       // sweep base servo, read distance, detect object
  ALIGN,      // pivot the chassis to face the angle the object was found at
  APPROACH,   // drive forward a measured distance to close the last few cm
  GRAB,       // lower arm, close gripper, raise arm
  RETREAT,    // reverse after grabbing
};

State state = IDLE;

// ── Non-blocking servo interpolation ──────────────────────────────
// Each servo channel tracks a current angle and a target angle.
// updateServos() moves current toward target at SERVO_DEG_PER_MS,
// so the arm moves smoothly instead of snapping.
struct ServoState {
  int current;  // current angle (degrees)
  int target;   // desired angle (degrees)
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
  setServoTarget(CH_BASE, BASE_CENTER);
  setServoTarget(CH_ARM, ARM_CARRY);
  setServoTarget(CH_GRIPPER, GRIPPER_OPEN);
}

// ── Per-state timing ──────────────────────────────────────────────
unsigned long stateMs = 0;  // entered-state timestamp

// Scan state: sweep direction and last reading
int   scanDir     = 1;     // +1 = sweeping right, -1 = sweeping left
float lastDist    = 0.0f;  // last distance reading (cm)
int   detectAngle = 90;    // base-servo angle the object was detected at (90 = straight ahead)

// Grab/retreat sub-phases (phased by servo completion)
uint8_t subPhase = 0;

// ── Encoder-terminated motion ─────────────────────────────────────
// Every driving state ends on a COUNT, never on a stopwatch.
long driveTarget = 0;   // |counts| each wheel must travel before the state is done

void resetEncoders() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}

// True once BOTH wheels have covered the target. Comparing the pair (rather than an average)
// means a stalled wheel keeps the state open instead of silently short-changing the distance.
bool driveDone() {
  long l = labs((long)encoders.getCountsLeft());
  long r = labs((long)encoders.getCountsRight());
  return (l >= driveTarget) && (r >= driveTarget);
}

void enterState(State s) {
  state = s;
  stateMs = millis();
  switch (s) {
    case IDLE:
      allStop();
      armCarry();
      showState("IDLE");
      break;
    case SCAN:
      allStop();
      setServoTarget(CH_ARM, ARM_CARRY);
      setServoTarget(CH_GRIPPER, GRIPPER_OPEN);
      // Start scan from current base position, sweeping right
      scanDir = 1;
      setServoTarget(CH_BASE, BASE_SCAN_MAX);
      showState("SCAN");
      break;
    case ALIGN: {
      // Pivot by (detectAngle − 90°). Positive = object to the servo's right → turn right.
      int offsetDeg = detectAngle - BASE_CENTER;
      driveTarget = (long)(abs(offsetDeg) * COUNTS_PER_DEG);
      resetEncoders();
      setServoTarget(CH_BASE, BASE_CENTER);   // re-centre the sensor as the body turns
      if (offsetDeg >= 0) motors.setSpeeds( TURN_SPEED, -TURN_SPEED);   // right pivot
      else                motors.setSpeeds(-TURN_SPEED,  TURN_SPEED);   // left pivot
      showState("ALIGN");
      break;
    }
    case APPROACH:
      driveTarget = (long)(APPROACH_CM * COUNTS_PER_CM);
      resetEncoders();
      motors.setSpeeds(APPROACH_SPEED, APPROACH_SPEED);
      showState("APPROACH");
      break;
    case GRAB:
      allStop();
      // Sequence: lower → close → raise (phased in updateGrab)
      setServoTarget(CH_ARM, ARM_DOWN);
      setServoTarget(CH_GRIPPER, GRIPPER_OPEN);  // ensure open before closing
      subPhase = 0;
      showState("GRAB");
      break;
    case RETREAT:
      driveTarget = (long)(RETREAT_CM * COUNTS_PER_CM);
      resetEncoders();
      motors.setSpeeds(-APPROACH_SPEED, -APPROACH_SPEED);
      setServoTarget(CH_ARM, ARM_UP);  // raise arm while retreating
      showState("RETREAT");
      break;
  }
}

// ── Scan logic ────────────────────────────────────────────────────
// 10 µs trigger pulse, then measure the echo pulse width. 58 µs per cm round trip.
// Returns distance in cm, or 0 on no echo (pulseIn timeout) — caller must check > 0.
float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long echoUs = pulseIn(ECHO_PIN, HIGH, (unsigned long)MAX_CM * 58UL);
  return echoUs / 58.0f;
}

void updateScan() {
  // Read distance. readDistanceCm() returns 0 when no echo (out of range) — must check > 0.
  // pulseIn() blocks up to ~12 ms at MAX_CM=200, acceptable here because we're
  // not driving during scan.
  float dist = readDistanceCm();

  if (dist > 0 && dist <= GRAB_DIST_CM) {
    // Object detected — remember WHERE the sensor was pointing, then turn to face it.
    // The base servo carries the sensor, so the object is at servos[CH_BASE].current, not
    // necessarily straight ahead.
    detectAngle = servos[CH_BASE].current;
    lastDist    = dist;
    Serial.print("Object at "); Serial.print(dist, 1);
    Serial.print(" cm, base angle "); Serial.println(detectAngle);
    enterState(ALIGN);
    return;
  }

  // If no object, continue sweeping. When base reaches target, reverse direction.
  if (servosAtTarget()) {
    if (scanDir > 0) {
      scanDir = -1;
      setServoTarget(CH_BASE, BASE_SCAN_MIN);
    } else {
      scanDir = 1;
      setServoTarget(CH_BASE, BASE_SCAN_MAX);
    }
  }
}

// ── Grab sub-sequence (phased by servo completion) ────────────────
void updateGrab() {
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
    case 2:  // wait for arm to raise → retreat
      if (servosAtTarget()) {
        enterState(RETREAT);
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

  // HC-SR04 pins — raw GPIO, we own the pinMode calls
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  // Initialize servos to carry pose
  servos[0].current = servos[0].target = BASE_CENTER;
  servos[1].current = servos[1].target = ARM_UP;
  servos[2].current = servos[2].target = GRIPPER_OPEN;
  for (uint8_t i = 0; i < 3; i++) writeServo(i, servos[i].current);

  enterState(IDLE);
  Serial.println("Ready. Press A to scan. B to abort.");
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
        enterState(SCAN);
      }
      break;

    case SCAN:
      updateScan();
      break;

    case ALIGN:
      // Pivot until the encoders say the turn is complete, then creep forward.
      if (driveDone()) {
        allStop();
        enterState(APPROACH);
      }
      break;

    case APPROACH:
      // Drive APPROACH_CM measured on the wheels, then grab.
      if (driveDone()) {
        allStop();
        enterState(GRAB);
      }
      break;

    case GRAB:
      updateGrab();
      break;

    case RETREAT:
      // Reverse RETREAT_CM measured on the wheels, then return to IDLE.
      if (driveDone()) {
        allStop();
        enterState(IDLE);
      }
      break;
  }
}
