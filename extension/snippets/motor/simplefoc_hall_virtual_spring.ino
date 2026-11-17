// snippet: simplefoc_hall_virtual_spring
// tags: motor, foc, bldc, simplefoc, hall, compliance, spring, damper, impedance, torque, proprioceptive
// boards: esp32
// library: SimpleFOC
// description: Active compliance via virtual spring-damper with SimpleFOC and Hall sensor position feedback.
//   ESP32 only as written: 25/26/27/14 and 32/33/34 are ESP32 GPIO numbers. On an AVR those are
//   not pins at all, and a Leonardo/Uno has neither the three spare external interrupts the Hall
//   sensor needs nor the timer budget for 3-phase PWM alongside anything else.
//   Hall inputs on GPIO 34-39 are INPUT-ONLY and have NO internal pull-ups — the sensor needs
//   external pull-ups (typically 4.7k to 3V3) on those pins or the states read as noise.
//   Parse serial commands with a NON-BLOCKING character reader. Serial.readStringUntil('\n')
//   blocks for up to 1000 ms waiting for a terminator, and loopFOC() must run every few hundred
//   microseconds to keep commutation aligned. One partial line stalls the field at a fixed
//   electrical angle: the motor lurches and the windings heat.
// prompts: active compliance, virtual spring, virtual damper, impedance control, proprioceptive, compliant robot, backdrivable motor, hall sensor foc, spring damper, variable stiffness, torque control hall, simplefoc compliance
// not-tags: avr, romi32u4, romi, leonardo, megaavr, nano33iot, rp2040

// Virtual impedance law: τ = K·(θ_eq − θ) − B·ω
//   K  = spring stiffness  (N·m/rad) — how hard the joint resists displacement
//   B  = damping coefficient (N·m·s/rad) — dissipates oscillations
//   θ_eq = equilibrium (set-point) position in radians
//   θ  = current joint angle  (from Hall sensor, via SimpleFOC)
//   ω  = angular velocity     (from Hall sensor, via SimpleFOC)
//
// Motor must be low-KV (high Kt, high torque per amp) and direct-drive or low gear ratio (≤6:1) to be backdrivable.
// High gear ratios kill backdrivability via friction and reflected inertia — regardless of motor KV.
// SimpleFOC runs FOC in TorqueControlType::voltage mode; motor.move() sets the
// quadrature voltage which is proportional to torque.
//
// Serial: send a float → sets new equilibrium position (rad).
//         send "K:<value>" → sets spring constant.
//         send "B:<value>" → sets damping coefficient.
// exact-include: SimpleFOC.h

#include <SimpleFOC.h>

// ── Hardware ─────────────────────────────────────────────────────────────────
// BLDCDriver3PWM(phA, phB, phC, en): 4th arg = gate driver enable pin (active HIGH).
// Omitting it leaves the enable line floating — driver may not power up.
BLDCDriver3PWM driver(25, 26, 27, 14);

// HallSensor(hallA, hallB, hallC, pole_pairs): NOT a quadrature encoder.
// Hall gives 6 coarse states per revolution; SimpleFOC interpolates velocity from edge timing.
// pole_pairs MUST match BLDCMotor(7) — mismatch causes wrong angle scaling.
HallSensor sensor = HallSensor(32, 33, 34, 7);

// BLDCMotor(n): n = pole PAIRS (not total poles). Must match HallSensor declaration above.
BLDCMotor motor = BLDCMotor(7);

// ── Compliance parameters ────────────────────────────────────────────────────
float kSpring  = 2.0;   // N·m/rad — increase for stiffer joint
float bDamp    = 0.05;  // N·m·s/rad — increase to suppress oscillation
float eqPos    = 0.0;   // equilibrium position in radians

// ── Hall interrupt handlers — must be global free functions ──────────────────
void doA() { sensor.handleA(); }
void doB() { sensor.handleB(); }
void doC() { sensor.handleC(); }

unsigned long prevMs = 0;

// ── Non-blocking line reader ─────────────────────────────────────────────────
// Accumulate one character per pass, act only on '\n'. Never waits for the rest of a line,
// so loopFOC() keeps its timing no matter how slowly the operator types.
char    cmdBuf[24];
uint8_t cmdIdx = 0;

void applyCommand(const char* c) {
  if      (strncmp(c, "K:", 2) == 0) kSpring = atof(c + 2);
  else if (strncmp(c, "B:", 2) == 0) bDamp   = atof(c + 2);
  else                               eqPos   = atof(c);
}

void pollSerial() {
  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == '\n') {
      cmdBuf[cmdIdx] = '\0';
      if (cmdIdx) applyCommand(cmdBuf);
      cmdIdx = 0;
    } else if (ch != '\r' && cmdIdx < sizeof(cmdBuf) - 1) {
      cmdBuf[cmdIdx++] = ch;
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Hall sensor: init then register all three interrupt handlers
  sensor.init();
  sensor.enableInterrupts(doA, doB, doC);   // all 3 Hall channels required

  // Driver + motor
  driver.voltage_power_supply = 12;
  driver.init();
  motor.linkSensor(&sensor);
  motor.linkDriver(&driver);

  // TorqueControlType::voltage: no current sensors required.
  // motor.move() sets quadrature voltage Vq (volts), NOT torque in N·m.
  // For true torque control, use TorqueControlType::dc_current or foc_current (needs current sensors).
  motor.torque_controller = TorqueControlType::voltage;
  motor.controller        = MotionControlType::torque;
  motor.voltage_limit     = 6;   // peak Vq = 6 V (half of 12 V supply) — protects windings

  motor.init();
  motor.initFOC();  // SimpleFOC: calibrate & align encoder (call once after init())
}

void loop() {
  motor.loopFOC();   // inner FOC loop — must be called every iteration

  // Virtual spring-damper outer loop
  float theta  = sensor.getAngle();       // current angle (rad)
  float omega  = sensor.getVelocity();    // angular velocity (rad/s)
  float torque = kSpring * (eqPos - theta) - bDamp * omega;
  motor.move(torque);                     // sets torque target

  // Serial: update equilibrium, K, or B — non-blocking, no String, no readStringUntil timeout
  pollSerial();

  // Print state at 10 Hz
  if (millis() - prevMs >= 100) {
    prevMs = millis();
    Serial.print("theta: ");  Serial.print(theta, 3);
    Serial.print("  omega: "); Serial.print(omega, 3);
    Serial.print("  tau: ");   Serial.println(torque, 3);
  }
}
