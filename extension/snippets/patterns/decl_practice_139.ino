// snippet: decl_practice_139
// tags: globals, declare, millis, nonblocking
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Declare float kp=0.5, ki=0.1, kd=0.05, integral=0.0, prevError=0.0, output=0.0 at global scope
// prompts: declare, float, integral=0, preverror=0, output=0, scope
// verified: compile-only
// source: kd-generated (id 139)

float kp = 0.5f, ki = 0.1f, kd = 0.05f, integral = 0.0f, prevError = 0.0f, output = 0.0f;

void setup() {
  pinMode(5, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs >= 10) {  // 100Hz update rate
    // Compute dt BEFORE updating prevMs (otherwise dt is always 0)
    float dt = (now - prevMs) / 1000.0f;
    prevMs = now;

    float setpoint = 100.0f;
    // Board-specific ADC: AVR = 5V/10-bit, others = 3.3V/12-bit
#if defined(ESP32) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_ARCH_RP2040)
    float measured = analogRead(A0) / 4095.0f * 100.0f;
#else
    float measured = analogRead(A0) / 1023.0f * 100.0f;
#endif
    float error = setpoint - measured;
    integral += error * dt;
    integral = constrain(integral, -200.0f, 200.0f);  // anti-windup
    float derivative = (error - prevError) / dt;
    prevError = error;
    output = kp * error + ki * integral + kd * derivative;
    output = constrain(output, 0.0f, 255.0f);
    analogWrite(5, (int)output);
    Serial.print("sp: "); Serial.print(setpoint);
    Serial.print(" pv: "); Serial.print(measured);
    Serial.print(" out: "); Serial.println(output);
  }
}
