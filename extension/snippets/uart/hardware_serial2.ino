// snippet: hardware_serial2
// tags: uart, serial2, serial1, hardware, mega, esp32, rx, tx, multiple
// boards: esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Use hardware Serial1 / Serial2 (Mega, ESP32) to talk to a second device
// prompts: hardware Serial2, multiple UART ports, Serial1 Serial2 Mega, second hardware UART, hardware serial ESP32

void setup() {
  Serial.begin(115200);   // USB monitor
  Serial1.begin(9600);    // hardware UART on pins 18(TX) 19(RX) on Mega; GPIO17/16 on ESP32
  Serial.println("Serial1 ready");
}

void loop() {
  if (Serial1.available()) {
    String line = Serial1.readStringUntil('\n');
    Serial.print("from device: ");
    Serial.println(line);
  }
  if (Serial.available()) {
    char c = Serial.read();
    Serial1.write(c);
  }
}
