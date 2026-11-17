// snippet: romi_fsm_manipulator_2servo
// tags: romi, romi32u4, fsm, state-machine, pca9685, servo, manipulator, arm, gripper, button, manual, step
// boards: romi32u4
// library: Romi32U4, PCA9685
// description: Manual button-stepped FSM for a 2-servo manipulator (arm lift + gripper)
//   on a Romi 32U4, driven by a PCA9685 over I2C. No driving — this is a manipulator-only
//   demo, ideal for calibrating servo angles and debugging the arm before adding motion.
//
//   FSM: HOME → (btnA) → ARM_DOWN → (btnA) → GRIP_CLOSE → (btnA) → ARM_UP → (btnA) → GRIP_OPEN → HOME.
//   Button B reverses one step at any time. Button C jumps directly to HOME.
//   Each servo move is non-blocking (interpolated over time so the arm doesn't jerk).
//
//   Manipulator servo mapping (PCA9685 channels):
//     ch0 = ARM     (lift 0–180°, 0 = down, 180 = up)
//     ch1 = GRIPPER (0 = fully open, 180 = fully closed)
//
//   Romi32U4 API:
//     Romi32U4ButtonA/B/C → getSingleDebouncedPress() → bool (edge detect, self-debouncing)
//     Romi32U4LCD lcd → clear(); gotoXY(col,row); print("text"); 8×2 chars
//     ledRed/ledYellow/ledGreen(bool) — free functions for status indicators
//
//   PCA9685: I2C address 0x40 (default). Romi SDA=pin2, SCL=pin3.
//     setPWMFreq(50) = standard hobby servo (20 ms period).
//     writeMicroseconds(ch, us) sends pulse width directly — 1000 µs ≈ 0°, 2000 µs ≈ 180°.
//   WHY PCA9685 instead of Servo.h:
//     Servo.h on AVR captures Timer1 — the same timer Romi32U4Motors uses for drive PWM.
//     Even though this sketch doesn't drive, keeping PCA9685 means you can add motors later
//     without a timer conflict. PCA9685 generates PWM in hardware over I2C.
// prompts: Romi 2 servo arm, Romi gripper button step, Romi manipulator manual, Romi PCA9685 arm gripper, Romi FSM arm gripper, Romi32U4 servo gripper button, Romi pick place manual step, Romi arm calibration
// not-tags: esp32, nano33iot, pico, mpu6050, encoder, drive
// exact-include: Romi32U4.h, Wire.h, Adafruit_PWMServoDriver.h

#include <Romi32U4.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>  // PCA9685 library (Adafruit PWM Servo Driver Library)

// ── Hardware objects ──────────────────────────────────────────────
Romi32U4ButtonA btnA;
Romi32U4ButtonB btnB;
Romi32U4ButtonC btnC;
Romi32U4LCD     lcd;

// PCA9685 at default I2C address 0x40.
// Romi32U4 I2C bus: SDA=pin2, SCL=pin3 — same bus as encoders/display, no conflict.
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// ── Servo channels & pulse limits ─────────────────────────────────
const uint8_t  CH_ARM     = 0;
const uint8_t  CH_GRIPPER = 1;

const uint16_t SERVO_MIN_US = 1000;  // 1000 µs ≈ 0°
const uint16_t SERVO_MAX_US = 2000;  // 2000 µs ≈ 180°

// ── Pose presets (degrees) ────────────────────────────────────────
// Calibrate these for your physical arm geometry.
// _DEG suffix: these are pose angles — plain ARM_UP/ARM_DOWN would collide with the State enum values below
const int ARM_UP_DEG    = 160;  // arm raised
const int ARM_DOWN_DEG  = 20;   // arm lowered to object height
const int GRIPPER_OPEN  = 0;    // fully open
const int GRIPPER_CLOSE = 130;  // fully closed (tune to your gripper)

// ── FSM ───────────────────────────────────────────────────────────
// enum State : uint8_t → 1-byte atomic write on AVR, safe even if read mid-cycle.
enum State : uint8_t {
  HOME,        // arm up, gripper open — resting pose
  ARM_DOWN,    // arm lowered, gripper still open
  GRIP_CLOSE,  // gripper closed around object, arm still down
  ARM_UP,      // arm raised with object
  GRIP_OPEN,   // gripper opened, object released
};

State state = HOME;

// ── Non-blocking servo interpolation ──────────────────────────────
// Each servo channel tracks a current angle and a target angle.
// updateServos() moves current toward target at SERVO_DEG_PER_MS,
// so the arm moves smoothly instead of snapping.
struct ServoState {
  int current;  // current angle (degrees)
  int target;   // desired angle (degrees)
};
ServoState servos[2];  // indexed by channel 0..1

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

  for (uint8_t i = 0; i < 2; i++) {
    int diff = servos[i].target - servos[i].current;
    if (diff == 0) continue;
    int step = (int)constrain(diff, -maxStep, maxStep);
    if (step == 0) step = (diff > 0) ? 1 : -1;  // ensure progress for small diffs
    servos[i].current += step;
    writeServo(i, servos[i].current);
  }
}

// Returns true when all servos have reached their targets.
bool servosAtTarget() {
  return servos[0].current == servos[0].target &&
         servos[1].current == servos[1].target;
}

// ── State helpers ─────────────────────────────────────────────────
void showState(const char* label) {
  lcd.clear();
  lcd.gotoXY(0, 0);
  lcd.print(label);
  Serial.println(label);
}

void enterState(State s) {
  state = s;
  switch (s) {
    case HOME:
      setServoTarget(CH_ARM, ARM_UP_DEG);
      setServoTarget(CH_GRIPPER, GRIPPER_OPEN);
      ledGreen(true); ledYellow(false); ledRed(false);  // Romi on-board yellow LED (true=on)
      showState("HOME");
      break;
    case ARM_DOWN:
      setServoTarget(CH_ARM, ARM_DOWN_DEG);
      // gripper stays as-is (open if coming from HOME, closed if coming from ARM_UP)
      ledGreen(false); ledYellow(true); ledRed(false);  // Romi on-board yellow LED (true=on)
      showState("ARM DOWN");
      break;
    case GRIP_CLOSE:
      setServoTarget(CH_GRIPPER, GRIPPER_CLOSE);
      ledGreen(false); ledYellow(true); ledRed(false);  // Romi on-board yellow LED (true=on)
      showState("GRIP CLOSE");
      break;
    case ARM_UP:
      setServoTarget(CH_ARM, ARM_UP_DEG);
      ledGreen(false); ledYellow(false); ledRed(true);  // Romi on-board yellow LED (true=on)
      showState("ARM UP");
      break;
    case GRIP_OPEN:
      setServoTarget(CH_GRIPPER, GRIPPER_OPEN);
      ledGreen(false); ledYellow(false); ledRed(true);  // Romi on-board yellow LED (true=on)
      showState("GRIP OPEN");
      break;
  }
}

// Forward step order: HOME → ARM_DOWN → GRIP_CLOSE → ARM_UP → GRIP_OPEN → HOME
State nextStep(State s) {
  switch (s) {
    case HOME:       return ARM_DOWN;
    case ARM_DOWN:   return GRIP_CLOSE;
    case GRIP_CLOSE: return ARM_UP;
    case ARM_UP:     return GRIP_OPEN;
    case GRIP_OPEN:  return HOME;
  }
  return HOME;
}

// Reverse step order: HOME ← ARM_DOWN ← GRIP_CLOSE ← ARM_UP ← GRIP_OPEN ← HOME
State prevStep(State s) {
  switch (s) {
    case HOME:       return GRIP_OPEN;
    case ARM_DOWN:   return HOME;
    case GRIP_CLOSE: return ARM_DOWN;
    case ARM_UP:     return GRIP_CLOSE;
    case GRIP_OPEN:  return ARM_UP;
  }
  return HOME;
}

// ── Setup ─────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  // PCA9685 init
  Wire.begin();
  pwm.begin();
  pwm.setPWMFreq(50);  // 50 Hz = standard hobby servo (20 ms period)
  // Do NOT set above ~330 Hz for analog servos — they overheat or stall.

  // Initialize servos to home pose
  servos[0].current = servos[0].target = ARM_UP_DEG;
  servos[1].current = servos[1].target = GRIPPER_OPEN;
  for (uint8_t i = 0; i < 2; i++) writeServo(i, servos[i].current);

  enterState(HOME);
  Serial.println("Ready. A=next step, B=prev step, C=home.");
}

// ── Main loop ─────────────────────────────────────────────────────
void loop() {
  updateServos();  // always advance servo motion

  // Button C: jump to HOME immediately
  if (btnC.getSingleDebouncedPress()) {
    enterState(HOME);
    return;
  }

  // Button A: advance one step (only after servos settle, to avoid skipping)
  if (btnA.getSingleDebouncedPress() && servosAtTarget()) {
    enterState(nextStep(state));
    return;
  }

  // Button B: reverse one step
  if (btnB.getSingleDebouncedPress() && servosAtTarget()) {
    enterState(prevStep(state));
    return;
  }
}
