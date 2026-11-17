// snippet: combined_130
// tags: combined, multi-sensor, millis
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Gas detection alarm: MQ-2 on A0
// prompts: detection, alarm, declare, float, globally, activate
// verified: compile-only
// source: kd-generated (id 130)

const int MQ2_PIN = A0;
const int BUZZER_PIN = 8;
const float THRESHOLD = 300.0;
float ppm = 0.0;
bool buzzerActive = false;
unsigned long buzzerStart = 0;
const unsigned long BUZZER_DURATION = 1000;

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  unsigned long now = millis();
  ppm = analogRead(MQ2_PIN) * (5.0 / 1023.0) * 1000.0; // Convert to ppm

  if (ppm > THRESHOLD) {
    if (!buzzerActive) {
      digitalWrite(BUZZER_PIN, HIGH);
      buzzerActive = true;
      buzzerStart = now;
    }
    if (now - buzzerStart < BUZZER_DURATION) {
      Serial.println("GAS ALERT");
    } else {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerActive = false;
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}
