// snippet: motor_dc_10
// tags: motor, dc, pwm, speed, direction
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Non-blocking motor brake: after receiving STOP command over serial, ramp PWM from current speed t…
// prompts: non-blocking, motor, brake, after, receiving, command
// verified: compile-only
// source: kd-generated (id 10)

const int ENA = 5;
const int IN1 = 6;
const int IN2 = 7;
const int STOP_COMMAND = 'S';
const unsigned long RAMP_TIME = 300;

int currentSpeed = 255;
bool braking = false;
unsigned long startBrakingMs = 0;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == STOP_COMMAND) {
      braking = true;
      startBrakingMs = millis();
    }
  }

  if (braking) {
    unsigned long now = millis();
    unsigned long elapsedTime = now - startBrakingMs;
    int newSpeed = map(elapsedTime, 0, RAMP_TIME, currentSpeed, 0);
    if (newSpeed <= 0) {
      newSpeed = 0;
      braking = false;
    }
    analogWrite(ENA, newSpeed);
    digitalWrite(IN1, newSpeed > 0 ? HIGH : LOW);
    digitalWrite(IN2, newSpeed > 0 ? LOW : HIGH);
  }
}
