// snippet: simplefoc_hall_compliance_multi
// tags: motor, foc, bldc, simplefoc, hall, compliance, multi, spring, damper, impedance, two, joint, leg
// boards: esp32
// library: SimpleFOC
// description: Two-motor active compliance for a 2-DOF joint (e.g. leg) — independent spring-damper per motor.
//   ESP32 only as written: the pin numbers below are ESP32 GPIOs, and two 3-PWM BLDC drivers
//   plus six hall interrupts is more timer and interrupt budget than an Uno or a Leonardo has.
//   Parse serial commands with a NON-BLOCKING character reader. Serial.readStringUntil('\n')
//   blocks for up to 1000 ms waiting for a terminator — and loopFOC() must run every few
//   hundred microseconds to keep commutation aligned. One partial line stalls commutation for a
//   full second: the field stops rotating, the windings sit energised at one electrical angle,
//   and the motor lurches and heats. This is the single most damaging thing you can put in a
//   FOC loop.
//   Hall inputs on GPIO 34-39 are INPUT-ONLY and have NO internal pull-ups — the hall sensor
//   needs external pull-ups (typically 4.7k to 3V3) on those pins or the states read as noise.
// prompts: two motor compliance, 2-dof compliance, multi motor spring damper, leg compliance, hip knee compliance, dual bldc hall, two joint impedance, locomotion compliance, compliant leg, two motor foc hall
// not-tags: avr, romi32u4, romi, megaavr, nano33iot

// Two independently compliant joints (e.g. hip + knee of a leg).
// Each joint runs its own virtual spring-damper:
//   τ_i = K_i·(θ_eq_i − θ_i) − B_i·ω_i
//
// Serial protocol (one command per line):
//   "H:<rad>"  hip equilibrium      "K:<rad>"  knee equilibrium
//   "HK:<k>"   hip stiffness        "BH:<b>"   hip damping
//   "KK:<k>"   knee stiffness       "BK:<b>"   knee damping
// exact-include: SimpleFOC.h

#include <SimpleFOC.h>

// ── Hip motor (motor A) ───────────────────────────────────────────────────────
// BLDCDriver3PWM(phA, phB, phC, en): 4th arg = gate driver enable pin (active HIGH)
BLDCDriver3PWM driverA(25, 26, 27, 14);
// HallSensor(hallA, hallB, hallC, pole_pairs): pole_pairs MUST match BLDCMotor constructor.
// NOT an Encoder — Hall gives 6 electrical states per mechanical revolution, not quadrature pulses.
// Requires 3 interrupt handlers (A, B, C); encoder only needs 2 (A, B).
HallSensor    sensorA = HallSensor(32, 33, 34, 7);
// BLDCMotor(n): n = pole PAIRS (not total poles). Must match HallSensor declaration.
BLDCMotor     motorA  = BLDCMotor(7);

// ── Knee motor (motor B) ──────────────────────────────────────────────────────
BLDCDriver3PWM driverB(16, 17, 18, 19);
HallSensor    sensorB = HallSensor(35, 36, 39, 7);
BLDCMotor     motorB  = BLDCMotor(7);

// ── Compliance parameters — hip ───────────────────────────────────────────────
float kHip  = 2.0,  bHip  = 0.05, eqHip  = 0.0;

// ── Compliance parameters — knee ──────────────────────────────────────────────
float kKnee = 2.0,  bKnee = 0.05, eqKnee = 0.0;

// ── Hall interrupt handlers ────────────────────────────────────────────────────
void doA_h() { sensorA.handleA(); }
void doB_h() { sensorA.handleB(); }
void doC_h() { sensorA.handleC(); }
void doA_k() { sensorB.handleA(); }
void doB_k() { sensorB.handleB(); }
void doC_k() { sensorB.handleC(); }

unsigned long prevMs = 0;

// ── Non-blocking line reader ───────────────────────────────────────────────────
// Accumulate one character per pass and act only on '\n'. Never waits for the rest of a line,
// so loopFOC() keeps its timing no matter how slowly the operator types.
char    cmdBuf[24];
uint8_t cmdIdx = 0;

void applyCommand(const char* c) {
  if      (strncmp(c, "HK:", 3) == 0) kHip   = atof(c + 3);
  else if (strncmp(c, "BH:", 3) == 0) bHip   = atof(c + 3);
  else if (strncmp(c, "KK:", 3) == 0) kKnee  = atof(c + 3);
  else if (strncmp(c, "BK:", 3) == 0) bKnee  = atof(c + 3);
  else if (strncmp(c, "H:",  2) == 0) eqHip  = atof(c + 2);   // 2-char prefixes last:
  else if (strncmp(c, "K:",  2) == 0) eqKnee = atof(c + 2);   // "KK:" must not match "K:"
}

void pollSerial() {
  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == '\n') {
      cmdBuf[cmdIdx] = '\0';
      applyCommand(cmdBuf);
      cmdIdx = 0;
    } else if (ch != '\r' && cmdIdx < sizeof(cmdBuf) - 1) {
      cmdBuf[cmdIdx++] = ch;
    }
  }
}

void initMotor(BLDCMotor& m, BLDCDriver3PWM& d, HallSensor& s,
               void(*hA)(), void(*hB)(), void(*hC)()) {
  s.init();
  s.enableInterrupts(hA, hB, hC);  // all 3 Hall channels required; missing any causes wrong velocity
  d.voltage_power_supply = 12;     // must match actual supply — FOC uses this for Vq normalisation
  d.init();
  m.linkSensor(&s);   // before m.init()
  m.linkDriver(&d);   // before m.init()
  // TorqueControlType::voltage: motor.move() sets quadrature voltage (V), not torque (N·m).
  // No current sensors required. Actual torque ∝ Vq / R_phase (motor-specific).
  m.torque_controller = TorqueControlType::voltage;
  m.controller        = MotionControlType::torque;
  m.voltage_limit     = 6;   // cap Vq at 6 V (half supply) — protects windings
  m.init();
  m.initFOC();  // electrical zero calibration per motor — each motor twitches once
}

void setup() {
  Serial.begin(115200);
  initMotor(motorA, driverA, sensorA, doA_h, doB_h, doC_h);
  initMotor(motorB, driverB, sensorB, doA_k, doB_k, doC_k);
}

void loop() {
  // Inner FOC loops — both must run every iteration; skipping one starves its commutation
  motorA.loopFOC();  // SimpleFOC: recompute FOC (call every loop BEFORE move())
  motorB.loopFOC();  // SimpleFOC: recompute FOC (call every loop BEFORE move())

  // Outer compliance loops
  float thetaH = sensorA.getAngle(), omegaH = sensorA.getVelocity();
  float thetaK = sensorB.getAngle(), omegaK = sensorB.getVelocity();

  motorA.move(kHip  * (eqHip  - thetaH) - bHip  * omegaH);
  motorB.move(kKnee * (eqKnee - thetaK) - bKnee * omegaK);

  // Serial command parser — non-blocking, no String, no readStringUntil timeout
  pollSerial();

  // Print at 10 Hz
  if (millis() - prevMs >= 100) {
    prevMs = millis();
    Serial.print("hip: "); Serial.print(thetaH, 3);
    Serial.print("  knee: "); Serial.println(thetaK, 3);
  }
}
