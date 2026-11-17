// snippet: combined_246
// tags: combined, multi-sensor, millis
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Soil moisture auto-watering: moisture on A0, water pump (relay) on pin 7
// prompts: moisture, auto-watering, moisture, water, (relay), declare
// verified: compile-only
// source: kd-generated (id 246)

#define SOIL_PIN  A0
#define RELAY_PIN 7

float moisture = 0.0;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Initially off
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  int raw = analogRead(SOIL_PIN);
  moisture = map(raw, 0, 1023, 0, 100);
  moisture = constrain(moisture, 0, 100);

  Serial.print("Soil moisture: "); Serial.print(moisture); Serial.println("%");

  if (moisture < 30) {
    digitalWrite(RELAY_PIN, HIGH);  // Turn on pump
    Serial.println("Watering...");
  } else if (moisture > 60) {
    digitalWrite(RELAY_PIN, LOW);  // Turn off pump
    Serial.println("Water pump off.");
  }
}
