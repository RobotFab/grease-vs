// snippet: virtual_compliance_brushed
// tags: motor, dc, l298n, compliance, spring, damper, impedance, encoder, pid, brushed, virtual, proprioceptive
// boards: avr, esp32, megaavr, nano33iot, romi32u4
// library: Encoder
// description: Active compliance for brushed DC motor using quadrature encoder — virtual spring-damper without FOC
// prompts: virtual spring brushed motor, compliance dc motor, spring damper encoder, brushed compliance, impedance control arduino, virtual compliance encoder, dc motor spring, encoder compliance, proprioceptive brushed, romi compliance, l298n spring

// Virtual impedance for a brushed DC motor + quadrature encoder.
//
// τ = K·(θ_eq − θ) − B·ω    (spring + damper)
//
// Since τ ∝ motor current ∝ applied voltage (at low speeds), we map torque
// to a PWM duty cycle directly:
//   duty = scale * (K*(eqTicks - ticks) - B*(ticks - prevTicks)/dt)
//
// This is valid when the gear ratio is low enough that back-EMF is small
// (backdrivable actuator regime) — the same condition required for proprioceptive
// force sensing in legged robots.
//
// Encoder resolution: e.g. 374 ticks/rev for Pololu 25D motor gearbox.
// Set eqTicks via Serial: send a long integer.
// Tune K and B for the specific motor + load.
// exact-include: Encoder.h

#include <Encoder.h>

// L298N: ENA=3 (PWM), IN1=4, IN2=5
const int ENA = 3, IN1 = 4, IN2 = 5;

// Quadrature encoder on interrupt pins 2 and 3
Encoder enc(2, 3);

// Virtual spring-damper parameters
float kSpring  = 0.08;   // duty/tick  — spring gain (tune to mass + stiffness)
float bDamp    = 0.4;    // duty·ms/tick — damping gain
long  eqTicks  = 0;      // equilibrium position (encoder ticks)

long  prevTicks = 0;
unsigned long prevMs = 0;

// ── Motor drive: duty in [-255, 255] ─────────────────────────────────────────
void setMotor(int duty) {
  duty = constrain(duty, -255, 255);
  if (duty > 0)      { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); }
  else if (duty < 0) { digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); duty = -duty; }
  else               { digitalWrite(IN1, LOW);   digitalWrite(IN2, LOW); }
  analogWrite(ENA, abs(duty));
}

void setup() {
  Serial.begin(115200);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
}

void loop() {
  unsigned long now = millis();
  unsigned long dt  = now - prevMs;
  if (dt < 5) return;   // 200 Hz control loop
  prevMs = now;

  long ticks = enc.read();
  long dTicks = ticks - prevTicks;
  prevTicks = ticks;

  // Spring + damper → duty
  float spring = kSpring * (float)(eqTicks - ticks);
  float damper = bDamp   * (float)dTicks / (float)dt;
  int   duty   = constrain((int)(spring - damper), -255, 255);
  setMotor(duty);

  // Serial: set equilibrium position
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.startsWith("EQ:"))     eqTicks = cmd.substring(3).toInt();
    else if (cmd.startsWith("K:")) kSpring  = cmd.substring(2).toFloat();
    else if (cmd.startsWith("B:")) bDamp    = cmd.substring(2).toFloat();
    else                           eqTicks  = cmd.toInt();
  }

  // Print at 20 Hz
  static unsigned long printMs = 0;
  if (now - printMs >= 50) {
    printMs = now;
    Serial.print("ticks: ");  Serial.print(ticks);
    Serial.print("  duty: "); Serial.println(duty);
  }
}
