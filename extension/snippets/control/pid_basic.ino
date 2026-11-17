// snippet: pid_basic
// tags: pid, controller, control, proportional, integral, derivative, feedback, setpoint, closed-loop
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Standard PID controller with proper I/D terms, anti-windup, and sensor read
//   PID formula: output = kp*error + ki*integral + kd*derivative
//   Anti-windup: clamp integral term to prevent saturation overshoot.
//   Derivative: (error - prevError) / dt, NOT (measurement - prevMeasurement).
//   All state (integral, prevError, prevMs) must be declared globally.
// prompts: PID controller, PID control loop, proportional integral derivative, feedback control, closed loop control, setpoint control, regulate temperature, speed control PID, balance control, PID tuning

float setpoint = 100.0;
float kp = 1.0, ki = 0.01, kd = 0.1;
float integral = 0.0, prevError = 0.0;
unsigned long prevMs = 0;

float pidCompute(float sp, float measured, float dt) {
  float error = sp - measured;
  integral += error * dt;
  integral  = constrain(integral, -200.0, 200.0);   // anti-windup clamp
  float derivative = (error - prevError) / dt;
  prevError = error;
  return kp * error + ki * integral + kd * derivative;
}

void setup() {
  Serial.begin(9600);
  prevMs = millis();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 10) return;   // 100 Hz control loop
  float dt = (now - prevMs) / 1000.0;
  prevMs = now;

  float measured = analogRead(A0) * (5.0 / 1023.0) * 100.0;   // sensor → physical units
  float output = pidCompute(setpoint, measured, dt);
  output = constrain(output, 0, 255);
  analogWrite(9, (int)output);

  Serial.print("sp: ");  Serial.print(setpoint);
  Serial.print(" pv: ");  Serial.print(measured);
  Serial.print(" out: "); Serial.println(output);
}
