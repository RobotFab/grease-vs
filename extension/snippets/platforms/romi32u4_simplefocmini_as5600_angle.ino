// snippet: romi32u4_simplefocmini_as5600_angle
// tags: romi, romi32u4, pololu, leonardo, simplefoc, simplefocmini, drv8313, bldc, foc, as5600, magnetic encoder, i2c, closed-loop, angle, position, hold, buzzer, wheels-off, no wheels
// boards: romi32u4
// library: Simple FOC, Romi32U4
// description: CLOSED-LOOP ANGLE control of a BLDC on a Romi 32U4 with the wheels removed:
//   SimpleFOCmini for the phases, AS5600 magnetic encoder over I2C for position, buzzer on beep
//   when the shaft settles. This is the flagship "Romi without wheels" sketch.
//
//   PINS — verified against the Leonardo digital_pin_to_timer_PGM and SimpleFOC's
//   atmega32u4_mcu.cpp. With the wheels off, exactly three hardware PWM pins are free:
//     9  TIMER1A -> IN1     10 TIMER1B -> IN2     5 TIMER3A -> IN3     12 -> EN (plain digital)
//   The buzzer keeps pin 6 (TIMER4D) and millis() keeps TIMER0.
//   NOT pin 11 and NOT pin 3 — both sit on TIMER0. SimpleFOC drops the TIMER0 prescaler to 1
//     to reach 32 kHz, so millis() would run 64x fast, silently. Pin 3 is also the Romi's SCL.
//
//   I2C — the AS5600 is at 0x36 and the Romi is the I2C MASTER on pins 2 (SDA) / 3 (SCL).
//   It does not collide with the onboard LSM6DS33 (0x6B), LIS3MDL (0x1E) or a PCA9685 (0x40).
//   Call Wire.begin() yourself and hand the bus to the sensor with sensor.init(&Wire) — the
//   no-argument sensor.init() calls Wire.begin() for you but then you cannot set the clock
//   first. 400 kHz matters: the position loop reads the encoder every single loopFOC().
//
//   NOT the Rob Tillaart AS5600 library here — SimpleFOC needs a sensor it can linkSensor(),
//     and it ships that driver as MagneticSensorI2C(AS5600_I2C). Two libraries fighting over
//     one device is the usual failure.
//   NOT delay() anywhere in loop(): loopFOC() must run every few hundred microseconds or the
//     windings sit energised at one electrical angle and heat up.
//   ORDER MATTERS in setup(): linkSensor -> driver.init -> linkDriver -> motor.init ->
//     motor.initFOC(). initFOC finds the electrical zero against the sensor; calling it before
//     the sensor or driver exists fails at runtime, not at compile time.
//
//   FLASH IS THE BINDING CONSTRAINT ON THIS RIG — measured on the 28672-byte ATmega32U4:
//     closed-loop FOC + AS5600, nothing else ............ 26106  (91%)
//     + Romi32U4Buzzer .................................. 28214  (98%)
//     + one Serial.print(float) and one atof() .......... 30162 (105%)  WILL NOT FIT
//   Printing or parsing a float drags in dtostrf/strtod and costs ~1.8 KB — more than the
//   entire application. On this rig:
//     NEVER Serial.print(aFloat), NEVER atof()/parseFloat()/Serial.parseFloat().
//     Float ARITHMETIC is free; only the print and parse paths cost. Prefer precomputed
//     float constants over converting ints at runtime, and wrap literals in F().
//   This sketch omits Serial entirely and reports through the buzzer, which is what leaves
//   room for the control loop. Adding Romi32U4LCD, Servo.h or a second heavy library
//   overflows the board no matter what else you cut.
// prompts: Romi SimpleFOCmini AS5600, Romi without wheels hold angle, Romi32U4 closed loop position BLDC, Romi FOC magnetic encoder, Romi hold shaft 90 degrees, Romi wheels off position control, SimpleFOCmini AS5600 Leonardo, Romi BLDC beep when settled, Romi 32U4 angle control brushless, Romi FOC i2c encoder buzzer
// not-tags: esp32, nano33iot, pico, rp2040, servo.h
// exact-include: SimpleFOC.h, Romi32U4.h, Wire.h

#include <SimpleFOC.h>
#include <Romi32U4.h>
#include <Wire.h>

// ── SimpleFOCmini (DRV8313): IN1, IN2, IN3, EN ────────────────────────
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 10, 5, 12);
BLDCMotor      motor  = BLDCMotor(7);          // pole pairs of YOUR motor

// ── AS5600 through SimpleFOC's own I2C sensor driver ──────────────────
MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);   // 0x36

// ── Romi hardware still present with the wheels off ───────────────────
Romi32U4Buzzer buzzer;    // Timer4 / pin 6 — no begin(), ready on construction

float target = 0.0f;      // radians — the only unit SimpleFOC accepts

// Settle detection: within tolerance, continuously, for a while.
const float SETTLE_RAD = 0.05f;    // ~3 degrees
const unsigned long SETTLE_MS = 300;
unsigned long inToleranceSince = 0;
bool announced = false;

void setup() {
  // No Serial.begin() — see the flash budget in the header. The buzzer is the status channel.
  Wire.begin();              // Romi is the I2C master
  Wire.setClock(400000);     // the position loop reads this every loopFOC()
  sensor.init(&Wire);
  motor.linkSensor(&sensor);

  driver.voltage_power_supply = 12.0f;   // set BEFORE init — everything normalises to it
  driver.init();
  motor.linkDriver(&driver);

  motor.controller = MotionControlType::angle;

  // Velocity PI (inner loop), then the position P (outer loop).
  motor.PID_velocity.P = 0.2f;
  motor.PID_velocity.I = 20.0f;
  motor.LPF_velocity.Tf = 0.01f;
  motor.P_angle.P = 20.0f;

  // In angle mode the position gain commands whatever speed closes the error,
  // so a large step slams the joint at full current unless these bound it.
  motor.velocity_limit = 20.0f;   // rad/s
  motor.voltage_limit  = 3.0f;

  motor.init();
  motor.initFOC();               // aligns the sensor — needs sensor + driver already up

  buzzer.playFrequency(660, 80, 12);   // ready
}

void loop() {
  // Both of these must run as fast as the loop allows. No delay(), ever.
  motor.loopFOC();
  motor.move(target);

  unsigned long now = millis();

  // ── Step through a few setpoints so the beep has something to announce ──
  // A text parser (atoi over a char buffer) fits here only if something else goes;
  // on the real rig the setpoint arrives as one ASCII byte from the ESP32 on Serial1,
  // which costs nothing. A whole-degree table keeps the flash for the control loop.
  // Radians, precomputed — 0, 90, 180, 90 degrees. Storing degrees as an int and
  // converting at runtime costs an int-to-float routine this sketch cannot afford.
  static const float TARGETS[] = { 0.0f, 1.5708f, 3.1416f, 1.5708f };
  static uint8_t idx = 0;
  static unsigned long stepMs = 0;
  if (now - stepMs >= 3000) {
    stepMs = now;
    idx = (idx + 1) & 0x03;
    target = TARGETS[idx];
    announced = false;
    inToleranceSince = 0;
  }

  // ── Beep once when the shaft settles on the target ──────────────────
  if (fabs(sensor.getAngle() - target) < SETTLE_RAD) {
    if (inToleranceSince == 0) inToleranceSince = now;
    if (!announced && now - inToleranceSince >= SETTLE_MS) {
      buzzer.playFrequency(880, 60, 12);   // returns immediately; does not block the loop
      announced = true;
    }
  } else {
    inToleranceSince = 0;
  }
}
