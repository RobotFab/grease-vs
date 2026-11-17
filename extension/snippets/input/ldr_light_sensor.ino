// snippet: ldr_light_sensor
// tags: ldr, light, photoresistor, analog, brightness, sensor
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Read ambient light level from an LDR (photoresistor) in a voltage divider.
//   Wiring: LDR one leg → 5V (or 3.3V), other leg → LDR_PIN and also to a 10kΩ resistor to GND.
//   Higher raw value = brighter light (for typical divider with LDR on top).
// prompts: LDR light sensor, photoresistor, ambient light sensing, light-dependent resistor, brightness measurement analog
//
// ---- CONFIGURE ----
#define LDR_PIN       A0
#define DARK_THRESH   200   // raw ADC below this = dark
#define BRIGHT_THRESH 800   // raw ADC above this = bright
// -------------------

void setup() {
  Serial.begin(115200);
}

void loop() {
  int raw = analogRead(LDR_PIN);
  float pct = raw / 1023.0f * 100.0f;

  Serial.print("Light: "); Serial.print(raw);
  Serial.print(" ("); Serial.print(pct, 1); Serial.print("%)  ");

  if      (raw < DARK_THRESH)   Serial.println("DARK");
  else if (raw > BRIGHT_THRESH) Serial.println("BRIGHT");
  else                          Serial.println("medium");

  delay(200);
}
