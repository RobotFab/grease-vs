// snippet: sensor_hall_583
// tags: hall, magnetic, sensor, motor, stall, detection, rpm, interrupt
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Motor stall detection: if Hall sensor on pin 2 shows 0 RPM for more than 1 second while motor com…
// prompts: motor, stall, detection, sensor, shows, second
// verified: compile-only
// source: kd-generated (id 583)

const int HALL_PIN = 2;
const int MOTOR_PIN = 5;
const unsigned long STALL_TIMEOUT = 1000;  // 1 second
unsigned long lastRpmTime = 0;
unsigned long stallStartTime = 0;
bool motorRunning = false;

void setup() {
  pinMode(HALL_PIN, INPUT_PULLUP);
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, 0);
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();

  // Check if motor is running
  if (digitalRead(HALL_PIN) == LOW) {
    lastRpmTime = now;
    motorRunning = true;
  } else if (motorRunning) {
    if (now - lastRpmTime >= STALL_TIMEOUT) {
      Serial.println("STALL");
      motorRunning = false;
      digitalWrite(MOTOR_PIN, 0);
    }
  }

  // Simulate motor command (replace with actual command)
  if (motorRunning) {
    analogWrite(MOTOR_PIN, 255);  // Full speed
  }
}
