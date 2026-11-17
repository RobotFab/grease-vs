// snippet: pico_onboard_temp
// tags: pico, rp2040, temperature, onboard, analogreadtemp, internal-sensor, celsius
// boards: rp2040
// library: none
// description: Read the RP2040's built-in temperature sensor with analogReadTemp() — no external parts
// prompts: Pico onboard temperature, RP2040 internal temp sensor, analogReadTemp, Raspberry Pi Pico temperature, chip temperature Pico
// exact-include: none

const unsigned long PRINT_MS = 1000;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < PRINT_MS) return;
  prevMs = now;

  float c = analogReadTemp();  // RP2040 internal sensor, degrees Celsius
  Serial.print("Temp: ");
  Serial.print(c, 1);
  Serial.println(" C");
}
