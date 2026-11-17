// snippet: motor_foc_281
// tags: motor, foc, bldc, simplefoc
// boards: esp32
// library: SimpleFOC
// description: SimpleFOC closed-loop VELOCITY control on ESP32, target set over Serial (rad/s).
//   driver.init() is NOT optional and motor.init() does not call it. motor.init() opens with
//   `if (!driver || !driver->initialized)` and returns 0 — but its complaint goes to
//   SIMPLEFOC_MOTOR_ERROR, which prints NOTHING unless you have called
//   SimpleFOCDebug::enable(&Serial). So the sketch compiles, uploads, runs, says nothing, and the
//   motor never turns. Init order is sensor → driver → motor → initFOC, and voltage_power_supply
//   must be set before driver.init() because FOC normalises Vq against it.
//   Read the target with a NON-BLOCKING character reader. Serial.readStringUntil('\n') blocks for
//   up to 1000 ms waiting for a terminator, and loopFOC() must run every few hundred microseconds
//   — a partial line stalls the field at one electrical angle and the windings heat.
// prompts: simplefoc velocity control, simplefoc serial target esp32, closed loop velocity bldc, set motor speed rad/s, encoder velocity foc
// verified: compile-only
// source: kd-generated (id 281)
// exact-include: SimpleFOC.h

#include <SimpleFOC.h>  // single header — all FOC classes included

// BLDCMotor(n): n is pole PAIRS, not total poles.
BLDCMotor motor = BLDCMotor(11);

// BLDCDriver3PWM(phA, phB, phC, en): 4th arg is the gate driver enable pin (active HIGH).
// Omitting it leaves the enable line floating — driver may not activate.
// Pins 25/26/27/22 are ESP32 GPIO numbers — not valid on AVR/megaavr/romi32u4.
BLDCDriver3PWM driver = BLDCDriver3PWM(25, 26, 27, 22);

// Encoder(pinA, pinB, PPR): quadrature encoder — NOT HallSensor, NOT MagneticSensorI2C.
// PPR = pulses per revolution of the encoder disk (2048 here = high resolution).
// Pins 34/35 are ESP32 input-only GPIOs — suitable for encoder signals.
Encoder encoder = Encoder(34, 35, 2048);

// ISR callbacks: SimpleFOC's Encoder class needs these registered so it can
// count edges in hardware interrupts. Without them, getAngle()/getVelocity()
// return stale or zero values because no interrupt ever fires.
void doA() { encoder.handleA(); }
void doB() { encoder.handleB(); }

float targetVelocity = 0.0;  // rad/s

// ── Non-blocking line reader ─────────────────────────────────────────────────
char    cmdBuf[16];
uint8_t cmdIdx = 0;

void pollSerial() {
  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == '\n') {
      cmdBuf[cmdIdx] = '\0';
      if (strncmp(cmdBuf, "VEL ", 4) == 0) targetVelocity = atof(cmdBuf + 4);
      cmdIdx = 0;
    } else if (ch != '\r' && cmdIdx < sizeof(cmdBuf) - 1) {
      cmdBuf[cmdIdx++] = ch;
    }
  }
}

void setup() {
  Serial.begin(115200);

  // encoder.init() must be called before enableInterrupts(), and both
  // before motor.init() — motor.init() reads the sensor to find initial angle.
  encoder.init();
  encoder.enableInterrupts(doA, doB);  // A and B only (no C for quadrature)

  // The driver must be configured and started itself — motor.init() does NOT do it.
  // Skip driver.init() and everything still compiles and calibrates; the motor just never turns.
  driver.voltage_power_supply = 12;   // must match the ACTUAL supply
  driver.init();

  motor.linkDriver(&driver);
  motor.linkSensor(&encoder);
  // MotionControlType::velocity → motor.move() target is in rad/s (NOT RPM)
  motor.controller    = MotionControlType::velocity;
  motor.voltage_limit = 6;   // cap Vq at half the supply — protects the windings
  motor.init();
  // initFOC() calibrates electrical zero — motor twitches once
  motor.initFOC();  // SimpleFOC: calibrate & align encoder (call once after init())
  motor.enable();  // explicit enable; initFOC() enables by default, but harmless to call again
}

void loop() {
  motor.loopFOC();                // inner FOC loop — must run every iteration
  motor.move(targetVelocity);     // velocity mode: target in rad/s

  pollSerial();   // "VEL <rad/s>" — non-blocking, no String, no readStringUntil timeout
}
