// snippet: combined_257
// tags: combined, multi-sensor, millis
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Security sensor: PIR on pin 3 (INPUT), buzzer on pin 8, LED on pin 13
// prompts: security, sensor, (input), buzzer, declare, alarmactive=false
// verified: compile-only
// source: kd-generated (id 257)

#define PIR_PIN   3
#define BUZZER_PIN  8
#define LED_PIN     13
#define ALARM_DURATION 3000

bool alarmActive = false;
unsigned long alarmEnd = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("PIR ready — allow 30s warm-up");
}

void loop() {
  if (digitalRead(PIR_PIN) == HIGH) {
    if (!alarmActive) {
      alarmActive = true;
      alarmEnd = millis() + ALARM_DURATION;
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(LED_PIN, HIGH);
      Serial.println("Alarm triggered");
    }
  } else if (alarmActive && millis() >= alarmEnd) {
    alarmActive = false;
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    Serial.println("Alarm ended");
  }
}
