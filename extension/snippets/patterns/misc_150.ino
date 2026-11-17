// snippet: misc_150
// tags: misc, millis, nonblocking
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Analog oversampling: read A0 sixteen times, declare long sum=0, float result globally
// prompts: analog, oversampling, sixteen, times, sum=0, float
// verified: compile-only
// source: kd-generated (id 150)

int analogPin = A0;
long sum = 0;
float result = 0.0f;

void setup() {
  Serial.begin(9600);
}

void loop() {
  static unsigned long t = 0;
  if (millis() - t >= 200) {
    t = millis();
    sum = 0;
    for (int i = 0; i < 16; i++) {
      sum += analogRead(analogPin);
    }
    // 16x oversampling of a 10-bit ADC yields +2 bits of resolution (12-bit).
    // Divide by 4 (>> 2) to scale back to 12-bit range (0-4095).
    // On 12-bit ADC boards (ESP32/RP2040), this gives 14-bit effective resolution.
    result = sum / 4.0f;
    Serial.print("average: "); Serial.println(result);
  }
}
