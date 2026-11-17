// snippet: motor_foc_282
// tags: motor, foc, bldc, simplefoc
// boards: esp32
// library: SimpleFOC
// description: SimpleFOC TORQUE (voltage) control on ESP32 with an AS5600, target set over Serial.
//   With the default TorqueControlType::voltage the target is a quadrature VOLTAGE, not newton
//   metres — there are no current sensors here to close a torque loop with.
//   Read the target with a NON-BLOCKING character reader. Serial.readStringUntil('\n') blocks for
//   up to 1000 ms waiting for a terminator, and loopFOC() must run every few hundred microseconds
//   — a partial line stalls the field at one electrical angle and the windings heat. In torque
//   mode this is worse than in velocity mode: nothing backs the current off.
// prompts: simplefoc torque control, simplefoc voltage mode, bldc torque esp32, quadrature voltage target, backdrivable bldc torque
// verified: compile-only
// source: kd-generated (id 282)
// exact-include: SimpleFOC.h

#include <SimpleFOC.h>  // single header — all FOC classes included

// BLDCMotor(n): n is pole PAIRS, not total poles.
BLDCMotor motor = BLDCMotor(7);

// BLDCDriver3PWM(phA, phB, phC): no enable pin — gate driver enable line is not driven.
// If your driver board has an EN pin, pass it as a 4th argument.
// Pins 25/26/27 are ESP32 GPIO numbers — not valid on AVR/megaavr/romi32u4.
BLDCDriver3PWM driver = BLDCDriver3PWM(25, 26, 27);

// AS5600_I2C preset: I2C addr 0x36, 12-bit, register 0x0C.
MagneticSensorI2C sensor(AS5600_I2C);

float targetTorque = 0.0;  // quadrature voltage (V), proportional to torque — see below

// ── Non-blocking line reader ─────────────────────────────────────────────────
char    cmdBuf[24];
uint8_t cmdIdx = 0;

void pollSerial() {
  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == '\n') {
      cmdBuf[cmdIdx] = '\0';
      if (strncmp(cmdBuf, "TORQUE ", 7) == 0) {
        targetTorque = constrain(atof(cmdBuf + 7), -3.0, 3.0);  // ±3 V quadrature voltage
      }
      cmdIdx = 0;
    } else if (ch != '\r' && cmdIdx < sizeof(cmdBuf) - 1) {
      cmdBuf[cmdIdx++] = ch;
    }
  }
}

void setup() {
  Serial.begin(115200);
  // sensor.init() and driver.init() must be called explicitly before motor.init().
  // motor.init() does NOT call them implicitly.
  sensor.init();
  driver.voltage_power_supply = 12;   // must match the ACTUAL supply — FOC normalises Vq against it
  driver.init();
  motor.linkDriver(&driver);
  motor.linkSensor(&sensor);
  motor.voltage_limit = 6;            // hard cap on Vq regardless of what the operator types
  // MotionControlType::torque with default TorqueControlType::voltage:
  //   motor.move() sets quadrature voltage Vq (volts), NOT torque in N·m.
  //   Actual torque ≈ Kτ × Vq / R_phase — motor-specific, uncalibrated.
  //   Current-sensor torque modes (dc_current / foc_current) give true torque but need hardware.
  motor.controller = MotionControlType::torque;
  motor.init();
  motor.initFOC();  // electrical zero calibration — motor twitches once
  motor.enable();
}

void loop() {
  motor.loopFOC();          // inner FOC loop — must run every iteration
  motor.move(targetTorque); // voltage mode: target is Vq in volts, capped by motor.voltage_limit

  pollSerial();   // "TORQUE <volts>" — non-blocking, no String, no readStringUntil timeout
}
