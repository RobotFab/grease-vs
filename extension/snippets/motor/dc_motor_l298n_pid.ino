// snippet: dc_motor_l298n_pid
// tags: motor, l298n, dc, pid, encoder, closed-loop, speed, control, feedback
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none (raw GPIO)
// description: L298N DC motor with encoder feedback and PID closed-loop speed control
//   Encoder ticks counted via interrupt. PID adjusts PWM to match target RPM.
//   Motor direction via IN1/IN2, speed via ENA (PWM). Encoder on interrupt pin 2.
// prompts: L298N PID motor, DC motor speed control PID, encoder motor control, closed loop motor, L298N encoder feedback, motor RPM control, DC motor PID speed
// not-tags: stepper, servo, brushless, esc
// exact-include:

// Motor pins (L298N)
#define ENA 9    // PWM speed
#define IN1 7
#define IN2 8

// Encoder
#define ENC_A 2  // interrupt-capable pin
const int TICKS_PER_REV = 360;

volatile long encoderTicks = 0;
long prevTicks = 0;
float targetRPM = 120.0;
float kp = 1.5, ki = 0.3, kd = 0.05;
float integral = 0.0, prevError = 0.0;
int pwmOut = 0;
unsigned long prevMs = 0;

void encoderISR() { encoderTicks++; }

void setMotor(int speed) {
  if (speed >= 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    speed = -speed;
  }
  analogWrite(ENA, constrain(speed, 0, 255));
}

void setup() {
  Serial.begin(115200);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENC_A, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), encoderISR, RISING);
  prevMs = millis();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 50) return;   // 20 Hz PID update
  float dt = (now - prevMs) / 1000.0;
  prevMs = now;

  // Measure RPM from encoder ticks
  noInterrupts();
  long ticks = encoderTicks;
  interrupts();
  long dTicks = ticks - prevTicks;
  prevTicks = ticks;
  float measuredRPM = (dTicks / (float)TICKS_PER_REV) * (60.0 / dt);

  // PID
  float error = targetRPM - measuredRPM;
  integral += error * dt;
  integral  = constrain(integral, -500.0, 500.0);
  float derivative = (error - prevError) / dt;
  prevError = error;
  float output = kp * error + ki * integral + kd * derivative;

  pwmOut = constrain((int)output, 0, 255);
  setMotor(pwmOut);

  Serial.print("target: "); Serial.print(targetRPM);
  Serial.print(" rpm: ");   Serial.print(measuredRPM);
  Serial.print(" pwm: ");   Serial.println(pwmOut);
}
