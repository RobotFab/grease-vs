// snippet: analog_read_print
// tags: analog, analogread, adc, potentiometer, sensor, a0, basics
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Read analog pin A0 and print raw ADC value and converted voltage
// prompts: analogRead, read analog pin, ADC reading, potentiometer value, sensor voltage, read A0

void setup() {
  Serial.begin(9600);
}

void loop() {
  int raw = analogRead(A0);
  float voltage = raw * (5.0 / 1023.0);   // 3.3 / 4095.0 for 3.3V 12-bit boards
  Serial.print("raw: ");
  Serial.print(raw);
  Serial.print("  voltage: ");
  Serial.println(voltage, 3);
  delay(100);
}
