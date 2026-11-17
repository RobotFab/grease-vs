// snippet: rain_sensor
// tags: rain, water, moisture, analog, digital, sensor, weather
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Read a rain/water detection sensor module. Most modules provide both a digital
//   output (threshold set by onboard pot) and an analog output (raw water coverage level).
//   Wiring: AO → RAIN_AO_PIN, DO → RAIN_DO_PIN, VCC → 5V, GND → GND.
// prompts: rain detection sensor, water sensor module, rainfall detector, weather moisture sensor, rain alert digital analog
//
// ---- CONFIGURE ----
#define RAIN_AO_PIN   A0   // analog output — lower value = more water
#define RAIN_DO_PIN    2   // digital output — LOW when rain detected (threshold via pot)
// -------------------

void setup() {
  Serial.begin(115200);
  pinMode(RAIN_DO_PIN, INPUT);
}

void loop() {
  int  raw      = analogRead(RAIN_AO_PIN);
  bool raining  = !digitalRead(RAIN_DO_PIN);   // active LOW

  Serial.print("Rain level: "); Serial.print(raw);
  Serial.print("  Digital: "); Serial.println(raining ? "WET" : "dry");
  delay(500);
}
