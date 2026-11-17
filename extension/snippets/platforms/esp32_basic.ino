// snippet: esp32_basic
// tags: esp32, led, blink, gpio, analog, adc, wemos
// boards: esp32
// library: none
// description: Basic ESP32 GPIO — LED blink and 12-bit ADC read (0-4095)
// prompts: ESP32 blink, ESP32 GPIO, ESP32 ADC, ESP32 analog read, ESP32 LED, Wemos D1 ESP32, ESP32 basic sketch

unsigned long prevMs = 0;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);  // onboard LED on most ESP32 devboards
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  ledState = !ledState;
  digitalWrite(2, ledState);

  int raw = analogRead(34);  // GPIO34 is input-only, good for ADC
  float voltage = raw * (3.3 / 4095.0);  // 12-bit ADC, 3.3V ref
  Serial.print("adc: "); Serial.print(raw);
  Serial.print(" v: "); Serial.println(voltage);
}
