// snippet: romi32u4_external_hbridge
// tags: romi, romi32u4, pololu, hbridge, h-bridge, l298n, tb6612, external motor, auxiliary motor, brushed dc, pwm, timer3, payload
// boards: romi32u4
// library: Romi32U4
// description: Drive an EXTERNAL brushed DC motor from a Romi 32U4 through an L298N/TB6612
//   H-bridge, while the Romi's own drive motors keep running. Auger/brush/lift motor pattern.
//
//   THE PIN PROBLEM — this is the whole difficulty of the task.
//   The Romi 32U4 board already owns almost every pin. Verified against the library source:
//     9, 10          drive motor PWM      (Timer1, Romi32U4Motors)
//     15, 16         drive motor direction
//     7, 8, 23, IO_E2  wheel encoders     (Romi32U4Encoders)
//     14, 17, IO_D5  buttons A / C / B    (also LCD db4/db5/db7)
//     4, 11, 13      LCD rs / e / db6
//     6              buzzer               (Timer4)
//     2, 3           I2C SDA / SCL
//   Leonardo/32U4 hardware-PWM pins are 3, 5, 6, 9, 10, 11, 13 — and every one of them is
//   taken EXCEPT pin 5.
//
//   PIN 5 IS THE ONLY FREE HARDWARE-PWM PIN ON THE ROMI 32U4, and it is on Timer3,
//   which is independent of Timer1 (wheels) and Timer4 (buzzer). So analogWrite(5, x)
//   does NOT disturb motors.setSpeeds() — the two motors run on separate timers.
//   Direction pins can be any free digital pin: A2 (20) and A3 (21) are broken out and unused.
//
//   NOT: Servo.h — it takes Timer1, the same timer Romi32U4Motors uses for wheel PWM.
//   NOT: analogWrite(9) or analogWrite(10) — those are the Romi's own drive PWM outputs.
//   NOT: pins 7, 8, 23 — writing them corrupts the encoder counts.
// prompts: Romi external motor, Romi H-bridge, Romi L298N, Romi extra motor, Romi auxiliary motor, Romi drive second motor, Romi TB6612, Romi brush motor, Romi payload motor, Romi32U4 additional DC motor
// not-tags: esp32, nano33iot, pico
// exact-include: Romi32U4.h

#include <Romi32U4.h>

// ── Romi's own hardware ───────────────────────────────────────────────
Romi32U4Motors   motors;
Romi32U4Encoders encoders;
Romi32U4ButtonA  btnA;

// ── External H-bridge (L298N channel A, or TB6612 channel A) ──────────
const uint8_t AUX_PWM  = 5;   // ENA  — Timer3, the only free PWM pin (see header)
const uint8_t AUX_IN1  = 20;  // IN1  — A2, free digital
const uint8_t AUX_IN2  = 21;  // IN2  — A3, free digital

// analogWrite() on AVR is 8-bit: 0..255. This is the H-bridge's range,
// NOT the Romi's — Romi32U4Motors::setSpeeds clamps to ±300 (allowTurbo(true) → ±400).
const int AUX_MAX_PWM = 255;

// Drive speed for the Romi's own wheels.
const int DRIVE_SPEED = 100;   // −300..300

// ── External motor control ────────────────────────────────────────────
// speed: −255..255. Negative = reverse. Zero = coast.
void auxMotorSetSpeed(int speed) {
  speed = constrain(speed, -AUX_MAX_PWM, AUX_MAX_PWM);

  if (speed > 0) {
    digitalWrite(AUX_IN1, HIGH);
    digitalWrite(AUX_IN2, LOW);
  } else if (speed < 0) {
    digitalWrite(AUX_IN1, LOW);
    digitalWrite(AUX_IN2, HIGH);
    speed = -speed;
  } else {
    // Both LOW = coast. Both HIGH would be an active brake.
    digitalWrite(AUX_IN1, LOW);
    digitalWrite(AUX_IN2, LOW);
  }
  analogWrite(AUX_PWM, speed);  // Timer3 — does not touch Timer1/wheels
}

void auxMotorStop() {
  digitalWrite(AUX_IN1, LOW);
  digitalWrite(AUX_IN2, LOW);
  analogWrite(AUX_PWM, 0);
}

// ── Non-blocking ramp so the payload motor does not slam to full power ─
int auxTarget  = 0;
int auxCurrent = 0;
unsigned long auxMs = 0;

void auxUpdate() {
  unsigned long now = millis();
  if (now - auxMs < 10) return;   // 100 Hz ramp
  auxMs = now;

  if (auxCurrent == auxTarget) return;
  int step = (auxTarget > auxCurrent) ? 5 : -5;
  auxCurrent += step;
  // Do not overshoot the target on the last step.
  if ((step > 0 && auxCurrent > auxTarget) || (step < 0 && auxCurrent < auxTarget)) {
    auxCurrent = auxTarget;
  }
  auxMotorSetSpeed(auxCurrent);
}

bool running = false;

void setup() {
  Serial.begin(9600);

  pinMode(AUX_PWM, OUTPUT);
  pinMode(AUX_IN1, OUTPUT);
  pinMode(AUX_IN2, OUTPUT);
  auxMotorStop();

  Serial.println("Press A: wheels + external motor on/off.");
}

void loop() {
  auxUpdate();  // always advance the ramp

  if (btnA.getSingleDebouncedPress()) {
    running = !running;
    if (running) {
      motors.setSpeeds(DRIVE_SPEED, DRIVE_SPEED);  // Timer1
      auxTarget = 200;                             // Timer3 — both run at once
      Serial.println("RUNNING");
    } else {
      motors.setSpeeds(0, 0);
      auxTarget = 0;
      Serial.println("STOPPED");
    }
  }

  // Report both motors at 5 Hz so you can see they are independent.
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs >= 200) {
    prevMs = now;
    Serial.print("wheels="); Serial.print(running ? DRIVE_SPEED : 0);
    Serial.print("  aux=");  Serial.print(auxCurrent);
    Serial.print("  encL="); Serial.println(encoders.getCountsLeft());
  }
}
