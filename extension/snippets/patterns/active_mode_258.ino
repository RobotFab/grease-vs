// snippet: active_mode_258
// tags: active, serial, command, response
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Active sketch: TUNE <kp> updates proportional gain, HOLD <setpoint> engages controller
// prompts: active, sketch, updates, proportional, <setpoint>, engages
// verified: compile-only
// source: kd-generated (id 258)

#define SENSOR_PIN   A0
#define SETPOINT_PIN A1
#define OUTPUT_PIN   9

float kp = 1.0f;
float ki = 0.0f;  // integral gain (not tuned via serial, but available)
float kd = 0.0f;  // derivative gain
float setpoint = 0.0f;
float measured = 0.0f;
float output = 0.0f;

float integral = 0.0f, prevError = 0.0f;
bool controllerActive = false;
unsigned long prevMs = 0;

float pidUpdate(float sp, float pv, float dt) {
  float err = sp - pv;
  integral += err * dt;
  integral = constrain(integral, -200.0f, 200.0f);  // anti-windup
  float deriv = (err - prevError) / dt;
  prevError = err;
  return kp * err + ki * integral + kd * deriv;
}

void setup() {
  Serial.begin(9600);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(SETPOINT_PIN, INPUT);
  prevMs = millis();
  Serial.println("Send: TUNE <kp>, HOLD <setpoint>, FREE, REPORT");
}

void loop() {
  unsigned long now = millis();
  float dt = (now - prevMs) / 1000.0f;
  prevMs = now;

  if (controllerActive && dt > 0.0f) {
    // Read sensor: convert ADC to 0-100 range
    measured = analogRead(SENSOR_PIN) * (100.0f / 1023.0f);
    output = pidUpdate(setpoint, measured, dt);
    output = constrain(output, 0.0f, 255.0f);
    analogWrite(OUTPUT_PIN, (int)output);
  }

  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd.startsWith("TUNE")) {
      // Parse kp from serial argument
      int spaceIdx = cmd.indexOf(' ');
      if (spaceIdx > 0) {
        float val = cmd.substring(spaceIdx + 1).toFloat();
        kp = constrain(val, 0.1f, 10.0f);
        Serial.print("KP set to: "); Serial.println(kp);
      }
    } else if (cmd.startsWith("HOLD")) {
      // Parse setpoint from serial argument
      int spaceIdx = cmd.indexOf(' ');
      if (spaceIdx > 0) {
        float val = cmd.substring(spaceIdx + 1).toFloat();
        setpoint = constrain(val, 0.0f, 100.0f);
      }
      controllerActive = true;
      integral = 0.0f;  // reset integral on new setpoint
      prevError = 0.0f;
      Serial.print("Controller engaged with setpoint: "); Serial.println(setpoint);
    } else if (cmd == "FREE") {
      controllerActive = false;
      analogWrite(OUTPUT_PIN, 0);
      Serial.println("Controller disengaged");
    } else if (cmd == "REPORT") {
      Serial.print("KP: "); Serial.println(kp);
      Serial.print("Setpoint: "); Serial.println(setpoint);
      Serial.print("Measured: "); Serial.println(measured);
      Serial.print("Output: "); Serial.println(output);
    }
  }
}
