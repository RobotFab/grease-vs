// snippet: romi32u4_simplefocmini_velocity
// tags: romi, romi32u4, pololu, leonardo, simplefoc, simplefocmini, drv8313, bldc, foc, as5600, closed-loop, velocity, speed, rpm, pid, buzzer, serial1, esp32, wheels-off, no wheels
// boards: romi32u4
// library: Simple FOC
// description: CLOSED-LOOP VELOCITY control of a BLDC on a Romi 32U4 with the wheels removed —
//   SimpleFOCmini, AS5600 for feedback, speed commanded as single ASCII bytes from an ESP32-C6
//   on Serial1, buzzer still alive. Velocity mode holds a commanded rad/s under changing load;
//   open loop cannot, because nothing tells it the rotor fell behind.
//
//   VELOCITY IS ONE LOOP, ANGLE IS TWO. In MotionControlType::velocity the only tuning that
//   matters is PID_velocity (P and I) and the LPF_velocity filter. P_angle is unused — setting
//   it here does nothing. Start P = 0.2, I = 20, Tf = 0.01 and raise P until the shaft buzzes,
//   then back off. Tf too small lets encoder noise into the current command; too large adds lag.
//   voltage_limit is the current limit; velocity_limit does not apply in velocity mode.
//
//   PINS — with the wheels off exactly three hardware PWM pins are free (verified against the
//   Leonardo digital_pin_to_timer_PGM and SimpleFOC's atmega32u4_mcu.cpp):
//     9 TIMER1A -> IN1   10 TIMER1B -> IN2   5 TIMER3A -> IN3   12 -> EN   6 TIMER4D -> buzzer
//   NOT pin 11, NOT pin 3 — both live on TIMER0, the timer millis() counts on, and SimpleFOC
//     drops the TIMER0 prescaler to 1 for 32 kHz. millis() would run 64x fast, silently.
//     Pin 3 is also the Romi's I2C SCL.
//
//   FLASH — the 32U4 has 28672 bytes and SimpleFOC uses most of them. Romi32U4Buzzer costs
//   ~2100 bytes (MML parser) and does not fit next to Serial1. Pin 6 is TIMER4D, so
//   analogWrite(6, 128) beeps the same buzzer for ~50 bytes; you get one pitch instead of
//   melodies. NEVER Serial.print(aFloat) or atof() on this rig — the float print/parse paths
//   cost ~1800 bytes, more than the whole application. Float arithmetic itself is free.
// prompts: Romi SimpleFOCmini velocity, Romi without wheels speed control, Romi32U4 closed loop velocity BLDC, Romi FOC rad per second, Romi wheels off spin at speed, SimpleFOCmini velocity PID Leonardo, Romi BLDC hold speed under load, Romi FOC velocity from ESP32, Romi 32U4 brushless rpm control, Romi velocity mode tuning
// not-tags: esp32, nano33iot, pico, rp2040, servo.h
// exact-include: SimpleFOC.h, Wire.h

#include <SimpleFOC.h>
#include <Wire.h>

BLDCDriver3PWM driver = BLDCDriver3PWM(9, 10, 5, 12);   // IN1, IN2, IN3, EN
BLDCMotor      motor  = BLDCMotor(7);                   // pole pairs of YOUR motor
MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);  // 0x36

const uint8_t BUZZER_PIN = 6;      // TIMER4D
unsigned long beepUntil  = 0;

void beep(uint16_t ms) {
  analogWrite(BUZZER_PIN, 128);
  beepUntil = millis() + ms;
}

float target = 0.0f;               // rad/s

void setup() {
  Serial1.begin(115200);           // the ESP32-C6 link; Serial (USB) stays free

  Wire.begin();                    // the Romi is the I2C master
  Wire.setClock(400000);           // the velocity loop reads the encoder every loopFOC()
  sensor.init(&Wire);
  motor.linkSensor(&sensor);

  driver.voltage_power_supply = 12.0f;   // BEFORE init — every voltage normalises to it
  driver.init();
  motor.linkDriver(&driver);

  motor.controller = MotionControlType::velocity;

  // The whole tuning surface in velocity mode.
  motor.PID_velocity.P  = 0.2f;
  motor.PID_velocity.I  = 20.0f;
  motor.LPF_velocity.Tf = 0.01f;
  motor.voltage_limit   = 3.0f;    // this is the current limit

  motor.init();
  motor.initFOC();                 // aligns the sensor — sensor and driver must exist first

  beep(80);                        // ready
}

void loop() {
  // Both run as fast as the loop allows. No delay(), ever — one stall leaves the
  // windings energised at a single electrical angle.
  motor.loopFOC();
  motor.move(target);

  unsigned long now = millis();

  if (beepUntil && now >= beepUntil) {
    analogWrite(BUZZER_PIN, 0);
    beepUntil = 0;
  }

  // ── One ASCII byte per command from the ESP32-C6 ───────────────────
  while (Serial1.available()) {
    char c = Serial1.read();
    switch (c) {
      case 'W': target += 2.0f; beep(30); break;   // faster
      case 'S': target -= 2.0f; beep(30); break;   // slower / reverse
      case ' ': target  = 0.0f; beep(60); break;   // stop
      default: continue;
    }
    target = constrain(target, -30.0f, 30.0f);
  }
}
