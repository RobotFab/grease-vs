// snippet: motor_foc_280
// tags: motor, foc, bldc, simplefoc
// boards: esp32
// library: SimpleFOC
// description: SimpleFOC closed-loop ANGLE control on ESP32 with an AS5600 magnetic encoder.
//   Angle mode moves the shaft to a commanded position in radians. Set voltage_power_supply to
//   the real supply and cap voltage_limit and velocity_limit BEFORE motor.init(): in angle mode
//   the default position P gain drives the motor at whatever speed closes the error, so a large
//   step command slams the joint at full current unless velocity_limit bounds it.
//   Read the target with a NON-BLOCKING character reader. Serial.parseFloat() blocks for up to
//   1000 ms waiting for a non-digit terminator, and loopFOC() must run every few hundred
//   microseconds — one stall leaves the windings energised at one electrical angle.
// prompts: simplefoc angle control, simplefoc position control, AS5600 angle esp32, magnetic encoder foc, closed loop angle bldc, move motor to angle radians
// verified: compile-only
// source: kd-generated (id 280)
// exact-include: SimpleFOC.h

#include <SimpleFOC.h>  // single header — pulls in all FOC classes; no sub-headers needed

// BLDCMotor(n): n is pole PAIRS, not total poles. A 14-pole motor → 7 pairs.
// Wrong pair count causes stuttering or no torque after initFOC().
BLDCMotor motor = BLDCMotor(7);

// BLDCDriver3PWM(phA, phB, phC[, en]): drives 3-phase gate signals — NOT an L298N H-bridge.
// Pins 25/26/27 are ESP32 GPIO numbers — not valid on AVR/megaavr/romi32u4.
BLDCDriver3PWM driver(25, 26, 27);

// MagneticSensorI2C(AS5600_I2C): AS5600_I2C is a SimpleFOC built-in preset
// (I2C addr 0x36, 12-bit resolution, register 0x0C, 4-bit low byte).
// Wire.h is NOT needed separately — SimpleFOC initialises I2C internally.
// NOT: analogRead() on OUT pin — use I2C mode here.
MagneticSensorI2C sensor(AS5600_I2C);

float target = 0.0;  // radians

// ── Non-blocking line reader ─────────────────────────────────────────────────
// One character per pass, act on '\n'. Never waits, so loopFOC() keeps its timing.
char    cmdBuf[16];
uint8_t cmdIdx = 0;

void pollSerial() {
  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == '\n') {
      cmdBuf[cmdIdx] = '\0';
      if (cmdIdx) target = atof(cmdBuf);   // plain float in radians
      cmdIdx = 0;
    } else if (ch != '\r' && cmdIdx < sizeof(cmdBuf) - 1) {
      cmdBuf[cmdIdx++] = ch;
    }
  }
}

void setup() {
  Serial.begin(115200);

  // linkSensor / linkDriver must be called BEFORE motor.init()
  motor.linkSensor(&sensor);
  motor.linkDriver(&driver);

  // Init order is fixed: sensor → driver → motor → FOC calibration.
  // Swapping order causes undefined behaviour or calibration failure.
  sensor.init();
  driver.voltage_power_supply = 12;   // must match the ACTUAL supply — FOC normalises Vq against it
  driver.init();

  // MotionControlType::angle → motor.move() target is in radians.
  // Set the mode and the limits BEFORE motor.init() so init() sizes its PIDs against them.
  motor.controller    = MotionControlType::angle;
  motor.voltage_limit = 6;     // cap Vq at half the supply — protects the windings
  motor.velocity_limit = 10;   // rad/s ceiling on the way to the target — without this a large
                               // step command slams the joint at whatever speed the P gain asks for
  motor.init();
  // initFOC() finds the electrical zero angle — motor twitches once here. Normal.
  // Blocks until calibration completes (≈ 1–2 s with magnetic sensor).
  motor.initFOC();  // SimpleFOC: calibrate & align encoder (call once after init())
}

void loop() {
  motor.loopFOC();    // inner FOC commutation — must run every loop iteration, no exceptions
  motor.move(target); // outer control: angle mode → target in radians (unwrapped, not 0–2π)

  static unsigned long prevMs = 0;
  if (millis() - prevMs >= 200) {
    prevMs = millis();
    Serial.print("Sensor Angle: ");
    Serial.println(sensor.getAngle());  // cumulative radians, unwrapped (can exceed 2π)
  }

  pollSerial();   // non-blocking; never stalls the commutation loop
}
