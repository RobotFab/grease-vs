// snippet: motor_dc_5
// tags: motor, dc, pwm, speed, direction
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Motor overcurrent protection: read A0 (current sense), if reading > 700 stop motor immediately an…
// prompts: motor, overcurrent, protection, (current, sense), reading
// verified: compile-only
// source: kd-generated (id 5)

const int MOTOR_PIN1 = 5;
const int MOTOR_PIN2 = 6;
const int CURRENT_SENSE_PIN = A0;
const int OVERCURRENT_THRESHOLD = 700;

void setup() {
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  static unsigned long t = 0;
  if (millis() - t >= 10) {  // Check every 10ms
    t = millis();
    int current = analogRead(CURRENT_SENSE_PIN);
    if (current > OVERCURRENT_THRESHOLD) {
      digitalWrite(MOTOR_PIN1, LOW);
      digitalWrite(MOTOR_PIN2, LOW);
      Serial.println("OVERCURRENT");
    }
  }
}
