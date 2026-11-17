// snippet: motor_dc_8
// tags: motor, dc, pwm, speed, direction
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: H-bridge soft start: ramp PWM on pin 5 from 0 to target speed over 500ms using millis
// prompts: h-bridge, start, target, speed, 500ms, command
// verified: compile-only
// source: kd-generated (id 8)

const int EN = 9;   // PWM enable (ENA on L298N)
const int IN1 = 7;
const int IN2 = 8;
const int TARGET_SPEED_PIN = A0;

int targetSpeed = 0;
int currentSpeed = 0;
unsigned long rampStartMs = 0;
const unsigned long RAMP_TIME = 500;

void setup() {
  Serial.begin(9600);
  pinMode(EN, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
}

void loop() {
  static unsigned long printMs = 0;
  const unsigned long PRINT_INTERVAL = 100;

  unsigned long now = millis();
  if (now - printMs >= PRINT_INTERVAL) {
    printMs = now;
    Serial.print("Current Speed: "); Serial.println(currentSpeed);
  }

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    if (cmd.startsWith("START ")) {
      targetSpeed = cmd.substring(6).toInt();
      rampStartMs = now;
      currentSpeed = 0;
    }
  }

  if (rampStartMs > 0) {
    unsigned long elapsed = now - rampStartMs;
    if (elapsed < RAMP_TIME) {
      currentSpeed = map(elapsed, 0, RAMP_TIME, 0, targetSpeed);
      motorForward(currentSpeed);
    } else {
      currentSpeed = targetSpeed;
      motorForward(currentSpeed);
      rampStartMs = 0;
    }
  }
}

void motorForward(int speed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(EN, constrain(speed, 0, 255));
}
