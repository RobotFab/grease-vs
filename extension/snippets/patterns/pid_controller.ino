// snippet: pid_controller
// tags: pid, controller, control, proportional, integral, derivative, feedback, tuning
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Generic PID controller class — non-blocking, millis() based
// prompts: PID controller, PID control loop, proportional integral derivative, motor speed control, feedback control, closed loop control, setpoint control, regulate temperature, balance control

unsigned long prevMs = 0;
float setpoint = 100.0;
float measured = 0.0;
float output   = 0.0;

float kp = 1.0, ki = 0.01, kd = 0.1;
float integral = 0.0, prevError = 0.0;

float pidUpdate(float sp, float pv, float dt) {
  float err = sp - pv;
  integral  += err * dt;
  integral   = constrain(integral, -200.0, 200.0);  // anti-windup
  float deriv = (err - prevError) / dt;
  prevError = err;
  return kp * err + ki * integral + kd * deriv;
}

void setup() {
  Serial.begin(9600);
  prevMs = millis();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 10) return;
  float dt = (now - prevMs) / 1000.0;
  prevMs = now;

  measured = analogRead(A0) * (5.0 / 1023.0) * 100.0;
  output = pidUpdate(setpoint, measured, dt);
  output = constrain(output, 0, 255);
  analogWrite(9, (int)output);

  Serial.print("sp: "); Serial.print(setpoint);
  Serial.print(" pv: "); Serial.print(measured);
  Serial.print(" out: "); Serial.println(output);
}
