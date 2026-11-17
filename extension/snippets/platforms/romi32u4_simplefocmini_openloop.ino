// snippet: romi32u4_simplefocmini_openloop
// tags: romi, romi32u4, pololu, leonardo, simplefoc, simplefocmini, mini, drv8313, bldc, foc, gimbal, openloop, open-loop, velocity, buzzer, timer, wheels-off, no wheels
// boards: romi32u4
// library: Simple FOC, Romi32U4
// description: Drive a BLDC motor from a SimpleFOCmini on a Romi 32U4 with THE WHEELS REMOVED,
//   in open loop, while the onboard buzzer keeps working. Open loop = no sensor at all.
//
//   THE PIN CHOICE IS THE WHOLE PROBLEM. Verified against the Leonardo variant's
//   digital_pin_to_timer_PGM and SimpleFOC's atmega32u4_mcu.cpp:
//     pin 3  -> TIMER0B   also the Romi's I2C SCL          UNUSABLE
//     pin 5  -> TIMER3A   free                             <- IN3
//     pin 6  -> TIMER4D   buzzer                           KEEP
//     pin 9  -> TIMER1A   was left wheel PWM, now free     <- IN1
//     pin 10 -> TIMER1B   was right wheel PWM, now free    <- IN2
//     pin 11 -> TIMER0A   also the LCD 'e' line            UNUSABLE
//     pin 13 -> TIMER4A   also LCD db6                     buzzer's timer
//
//   NOT pin 11. Pin 11 is on TIMER0, which is the timer millis() counts on. SimpleFOC's
//     _pinHighFrequency() sets the TIMER0 prescaler to 1 to reach 32 kHz, which makes millis()
//     run 64x fast. The sketch compiles, the motor spins, and every millis() timeout in the
//     program fires 64x early. There is no error message. Use 9, 10 and 5.
//   NOT pin 3 — it is I2C SCL on the Romi and also on TIMER0.
//   NOT Romi32U4Motors — the wheels are off; its Timer1 setup would fight the driver on 9/10.
//
//   The three phases land on two different timers (Timer1 for 9/10, Timer3 for 5). That is
//   fine for 3-PWM mode: SimpleFOC writes each phase with its own analogWrite(), it does not
//   need the carriers phase-aligned.
//
//   SimpleFOCmini = TI DRV8313: 8-35 V, 2.5 A per phase, inputs IN1/IN2/IN3 + EN.
//   EN is a plain digital pin — pass it as the 4th argument to BLDCDriver3PWM.
//   Set voltage_power_supply to the ACTUAL supply before driver.init(); FOC normalises every
//   voltage against it. In open loop, voltage_limit is the ONLY current limit — nothing detects
//   a stalled rotor — so keep it low (2-3 V on a small gimbal motor).
//   In open loop do NOT call linkSensor(), initFOC() or loopFOC(): all three exist to serve a
//   sensor, and there is none. The whole loop is motor.move(target).
// prompts: Romi SimpleFOCmini, Romi without wheels BLDC, Romi32U4 SimpleFOC open loop, Romi wheels off brushless, SimpleFOCmini Leonardo pins, Romi FOC which PWM pins, Romi BLDC buzzer still works, Romi 32U4 DRV8313, Romi spin bldc no encoder, Romi gimbal motor open loop
// not-tags: esp32, nano33iot, pico, rp2040, servo.h
// exact-include: SimpleFOC.h, Romi32U4.h

#include <SimpleFOC.h>
#include <Romi32U4.h>

// ── SimpleFOCmini on the three free PWM pins (see header) ─────────────
// BLDCDriver3PWM(IN1, IN2, IN3, EN)
BLDCDriver3PWM driver = BLDCDriver3PWM(9, 10, 5, 12);

// Pole pairs of YOUR motor. A 12N14P gimbal motor is 7. Wrong value here does not
// stop open loop from spinning, it just makes the commanded speed wrong.
BLDCMotor motor = BLDCMotor(7);

// ── Romi hardware that survives the wheels coming off ─────────────────
Romi32U4Buzzer  buzzer;   // Timer4 / pin 6 — untouched by the driver above
Romi32U4ButtonA btnA;
// No begin() on either — Romi32U4 classes are ready on construction.

float target = 0.0f;      // rad/s
bool  spinning = false;

void setup() {
  Serial.begin(9600);

  // Supply first: every other voltage is normalised against it.
  driver.voltage_power_supply = 12.0f;
  driver.init();
  motor.linkDriver(&driver);

  motor.controller = MotionControlType::velocity_openloop;

  // The only current limit that exists in open loop.
  motor.voltage_limit = 3.0f;

  motor.init();
  // No initFOC() — that aligns a sensor, and open loop has none.

  buzzer.play("!c32");            // one short blip: ready
  Serial.println("A = start/stop");
}

void loop() {
  if (btnA.getSingleDebouncedPress()) {
    spinning = !spinning;
    target = spinning ? 10.0f : 0.0f;   // rad/s
    buzzer.play(spinning ? "!>c32" : "!<c32");
    Serial.println(spinning ? "RUN" : "STOP");
  }

  // move() must be called as fast as possible — it steps the open-loop angle
  // from the time elapsed since the last call. Never delay() around it.
  motor.move(target);

  // Non-blocking status print at 2 Hz.
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs >= 500) {
    prevMs = now;
    Serial.print("target="); Serial.println(target);
  }
}
