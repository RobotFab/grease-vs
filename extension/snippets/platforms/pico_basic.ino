// snippet: pico_basic
// tags: pico, raspberry pi pico, rp2040, arduino, gpio, adc, led
// boards: rp2040
// library: none
// description: Raspberry Pi Pico with Arduino SDK — GPIO and 12-bit ADC
// prompts: Raspberry Pi Pico, RP2040 Arduino IDE, Pico GPIO ADC, Pi Pico LED blink, RP2040 starter sketch

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

unsigned long prevMs = 0;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);  // GPIO25 on Pico
  pinMode(A0, INPUT);            // GP26 = ADC0
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 500) return;
  prevMs = now;

  ledState = !ledState;
  digitalWrite(LED_BUILTIN, ledState);

  int raw = analogRead(A0);  // 12-bit: 0-4095
  float voltage = raw * (3.3 / 4095.0);
  Serial.print("adc: "); Serial.print(raw);
  Serial.print(" v: "); Serial.println(voltage);
}
