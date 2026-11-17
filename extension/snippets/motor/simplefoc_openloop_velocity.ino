// snippet: simplefoc_openloop_velocity
// tags: motor, foc, bldc, simplefoc, openloop, open-loop, velocity, gimbal, sensorless, no encoder
// boards: esp32
// library: SimpleFOC
// description: SimpleFOC OPEN-LOOP velocity on a gimbal BLDC — no sensor of any kind.
//   Open loop means exactly that: no encoder, no Hall sensor, no magnetic sensor. Do NOT call
//   linkSensor(), do NOT declare an Encoder/HallSensor/MagneticSensorI2C, and do NOT call
//   initFOC() — initFOC exists to find the electrical zero of a SENSOR, and there is none here.
//   Do NOT call loopFOC() either: it closes the current/position loop from sensor feedback, so
//   with no sensor it has nothing to do. The whole loop() is one motor.move(target).
//   Open loop spins the field at the commanded rate and TRUSTS the rotor to follow. Nothing
//   detects a stall, so voltage_limit is the only thing standing between a jammed rotor and a
//   burnt winding — keep it low (2-3 V on a gimbal motor) and treat it as a current setting,
//   not a speed setting. Speed comes from move(); torque headroom comes from voltage_limit.
//   Set voltage_power_supply to the ACTUAL supply before driver.init() — FOC normalises against
//   it, so a wrong value scales every voltage in the sketch.
//   Pins 25/26/27/14 are ESP32 GPIO numbers.
// prompts: simplefoc open loop velocity, open loop bldc, spin bldc without encoder, sensorless bldc velocity, gimbal motor open loop, velocity_openloop, bldc no sensor, simplefoc without encoder
// exact-include: SimpleFOC.h
// not-tags: avr, romi32u4, romi, leonardo, megaavr, nano33iot

#include <SimpleFOC.h>

// BLDCMotor(n): n = pole PAIRS, not total poles. A 14-pole gimbal motor is 7 pairs.
// In open loop the pair count sets how fast the field turns for a given target — get it wrong
// and the shaft turns at the wrong speed with no feedback to reveal it.
BLDCMotor motor = BLDCMotor(7);

// BLDCDriver3PWM(phA, phB, phC, en): 4th arg = gate driver enable pin (active HIGH).
BLDCDriver3PWM driver = BLDCDriver3PWM(25, 26, 27, 14);

// NO sensor object is declared. That is what makes this open loop.

float targetVelocity = 5.0;   // rad/s — about 48 rpm

// Non-blocking line reader: one character per pass, act on '\n'.
char    cmdBuf[16];
uint8_t cmdIdx = 0;

void pollSerial() {
  while (Serial.available()) {
    char ch = Serial.read();
    if (ch == '\n') {
      cmdBuf[cmdIdx] = '\0';
      if (cmdIdx) targetVelocity = atof(cmdBuf);   // plain float in rad/s
      cmdIdx = 0;
    } else if (ch != '\r' && cmdIdx < sizeof(cmdBuf) - 1) {
      cmdBuf[cmdIdx++] = ch;
    }
  }
}

void setup() {
  Serial.begin(115200);

  driver.voltage_power_supply = 12;   // must match the real supply
  driver.init();
  motor.linkDriver(&driver);
  // No motor.linkSensor() — there is no sensor.

  // In open loop this is the voltage actually applied to the windings, so it sets the current
  // and the heat. Low. A stalled open-loop motor draws this continuously and nothing notices.
  motor.voltage_limit = 3;

  motor.controller = MotionControlType::velocity_openloop;   // target in rad/s
  motor.init();
  // No motor.initFOC() — nothing to calibrate against.

  Serial.println("open-loop velocity ready; send a float in rad/s");
}

void loop() {
  // move() alone. It advances the electrical angle by target * dt every call, so it must be
  // called as fast and as regularly as possible — anything that blocks loop() makes the
  // commanded speed wrong, and there is no feedback to correct it.
  motor.move(targetVelocity);

  pollSerial();
}
