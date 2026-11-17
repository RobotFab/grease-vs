// snippet: romi32u4_foc_acs712_current
// tags: romi, romi32u4, pololu, leonardo, simplefoc, simplefocmini, drv8313, bldc, foc, as5600, acs712, current, current sensing, amps, overcurrent, buzzer, serial1, esp32, wheels-off, no wheels, flash
// boards: romi32u4
// library: Simple FOC, Romi32U4
// description: THE COMPLETE "Romi without wheels" RIG on one ATmega32U4: SimpleFOCmini closed-loop
//   angle from an AS5600, ACS712 current sensing, an audible over-current alarm, and a Serial1
//   command link from an ESP32 — 96% of flash, all of it fitting.
//
//   HOW IT FITS, WHICH IS THE ENTIRE LESSON. Measured on the 28672-byte ATmega32U4:
//     closed-loop FOC + AS5600, bare ....................... 26106  (91%)
//     + Romi32U4Buzzer ..................................... 28214  (98%)
//     + Serial1 as well .................................... 29212 (101%)  WILL NOT FIT
//     + Romi32U4Buzzer replaced by analogWrite on pin 6 ..... 27656  (96%)  fits, this sketch
//   Romi32U4Buzzer costs ~2100 bytes because it carries an MML parser and note tables. Pin 6 is
//   TIMER4D, so analogWrite(6, 128) puts a 50% square wave on the same buzzer for ~50 bytes.
//   You lose melodies and get one fixed pitch. On this rig that trade buys you the ESP32 link.
//   Turn it off with analogWrite(6, 0) — from a millis() deadline, never a delay().
//   NOT tone() — it wants its own timer and fights Timer4.
//   NOT Romi32U4Buzzer in the same sketch — it reconfigures Timer4 underneath analogWrite.
//   NEVER Serial.print(aFloat) or atof() here: the float print/parse paths cost ~1800 bytes,
//     more than this entire application. Integer milliamps, and F() around every literal.
//
//   PINS (verified against the Leonardo digital_pin_to_timer_PGM and SimpleFOC's
//   atmega32u4_mcu.cpp). With the wheels off, exactly three hardware PWM pins are free:
//     9 TIMER1A -> IN1   10 TIMER1B -> IN2   5 TIMER3A -> IN3   12 -> EN   6 TIMER4D -> buzzer
//   NOT pin 11 and NOT pin 3 — both are on TIMER0, which millis() counts on, and SimpleFOC
//     drops the TIMER0 prescaler to 1 to reach 32 kHz. millis() would silently run 64x fast.
//     Pin 3 is also the Romi's I2C SCL.
//
//   ACS712 MATH. It is ratiometric to its own 5 V supply, so the zero-current output is VCC/2,
//   not exactly 512 counts — calibrate it at boot with the motor idle and average many reads.
//     counts -> millivolts: 5000 mV / 1024 = 4.883 mV per count
//     millivolts -> milliamps: divide by the part's sensitivity
//       ACS712-05B = 185 mV/A    ACS712-20A = 100 mV/A    ACS712-30A = 66 mV/A
//   So mA = delta_counts * 4883 / 185, done in int32_t. Cast BEFORE multiplying: a 16-bit
//   int overflows at delta 7 counts.
//   analogRead() takes ~100 us and stalls loopFOC() while it runs. At 50 ms between samples
//   that is a 0.2% duty and harmless; calling it every loop is not.
// prompts: Romi ACS712 current sensing FOC, Romi without wheels current sensor, Romi32U4 overcurrent buzzer, Romi SimpleFOCmini ACS712, Romi measure motor current alarm, Romi FOC current limit beep, Romi wheels off ACS712 AS5600, Romi 32U4 current sense brushless, Romi ESP32 serial command FOC current, Romi flash too big SimpleFOC buzzer
// not-tags: esp32, nano33iot, pico, rp2040, servo.h
// exact-include: SimpleFOC.h, Wire.h

#include <SimpleFOC.h>
#include <Wire.h>

// ── SimpleFOCmini (DRV8313): IN1, IN2, IN3, EN ────────────────────────
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 10, 5, 12);
BLDCMotor      motor  = BLDCMotor(7);                      // pole pairs of YOUR motor
MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);  // 0x36, Romi is I2C master

// ── Buzzer as a bare Timer4 PWM pin, not the Romi32U4Buzzer class ─────
const uint8_t BUZZER_PIN = 6;      // TIMER4D
unsigned long beepUntil  = 0;

void beep(uint16_t ms) {
  analogWrite(BUZZER_PIN, 128);    // 50% duty on Timer4 — audible square wave
  beepUntil = millis() + ms;
}

// ── ACS712 ────────────────────────────────────────────────────────────
const uint8_t CURRENT_PIN     = A0;
const int16_t MV_PER_A        = 185;    // ACS712-05B. 100 for -20A, 66 for -30A.
const int16_t OVERCURRENT_MA  = 2000;

int16_t zeroCounts = 512;   // replaced by the boot calibration
int16_t milliamps  = 0;

float target = 0.0f;        // radians

void setup() {
  Serial1.begin(115200);    // link to the ESP32-C6. Serial (USB) is the computer.

  Wire.begin();
  Wire.setClock(400000);    // the position loop reads the encoder every loopFOC()
  sensor.init(&Wire);
  motor.linkSensor(&sensor);

  driver.voltage_power_supply = 12.0f;   // set BEFORE init — all voltages normalise to it
  driver.init();
  motor.linkDriver(&driver);

  motor.controller = MotionControlType::angle;
  motor.PID_velocity.P  = 0.2f;
  motor.PID_velocity.I  = 20.0f;
  motor.LPF_velocity.Tf = 0.01f;
  motor.P_angle.P       = 20.0f;
  motor.velocity_limit  = 20.0f;
  motor.voltage_limit   = 3.0f;

  motor.init();
  motor.initFOC();          // aligns the sensor — needs sensor + driver already up

  // Calibrate the ACS712 zero point with the motor idle. Ratiometric part:
  // the real midpoint follows the 5 V rail, which is not exactly 5.000 V.
  int32_t acc = 0;
  for (uint8_t i = 0; i < 64; i++) acc += analogRead(CURRENT_PIN);
  zeroCounts = acc / 64;

  beep(80);                 // ready
}

void loop() {
  // Must run as fast as the loop allows. No delay(), ever.
  motor.loopFOC();
  motor.move(target);

  unsigned long now = millis();

  // Non-blocking beep-off. This is why beep() can be called from anywhere.
  if (beepUntil && now >= beepUntil) {
    analogWrite(BUZZER_PIN, 0);
    beepUntil = 0;
  }

  // ── Single ASCII bytes from the ESP32-C6 ──────────────────────────
  while (Serial1.available()) {
    char c = Serial1.read();
    switch (c) {
      case 'W': target =  1.5708f; beep(40); break;   // 90 deg
      case 'S': target =  0.0f;    beep(40); break;   // home
      case 'A': target =  3.1416f; beep(40); break;   // 180 deg
      case 'D': target = -1.5708f; beep(40); break;   // -90 deg
      default: break;                                 // ignore newlines and noise
    }
  }

  // ── Current at 20 Hz ──────────────────────────────────────────────
  static unsigned long sampleMs = 0;
  if (now - sampleMs >= 50) {
    sampleMs = now;
    int16_t delta = analogRead(CURRENT_PIN) - zeroCounts;
    // Cast to int32_t BEFORE the multiply: delta * 4883 overflows int16 at delta 7.
    milliamps = ((int32_t)delta * 4883) / MV_PER_A;

    if (milliamps > OVERCURRENT_MA || milliamps < -OVERCURRENT_MA) {
      beep(60);
      motor.target = 0;                 // back the command off
      Serial1.write('!');               // tell the ESP32, one byte, no formatting
    }
  }
}
