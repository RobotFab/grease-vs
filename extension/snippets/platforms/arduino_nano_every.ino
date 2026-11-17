// snippet: arduino_nano_every
// tags: nano-every, nano33, megaavr, atmega4809, platform, pinout, basics
// boards: megaavr
// library: none
// description: Nano Every starter — 20MHz ATmega4809, 3.3V tolerant, note pin differences from Nano
// prompts: Arduino Nano Every, ATmega4809 starter, Nano Every pinout, megaAVR 4809, Nano Every blink basic

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  Serial.println("Arduino Nano Every ready");
  Serial.print("F_CPU: "); Serial.println(F_CPU);

  // analogRead is 10-bit (0-1023) like classic Nano
  // PWM pins: 3, 5, 6, 9, 10 (same as Uno)
  // A6/A7 are ANALOG INPUT ONLY (no digitalRead/Write)
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH); delay(500);
  digitalWrite(LED_BUILTIN, LOW);  delay(500);
  float v = analogRead(A0) * (5.0f / 1023.0f);
  Serial.print("A0: "); Serial.println(v, 3);
}
