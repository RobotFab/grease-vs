// snippet: serial_hello
// tags: serial, print, monitor, basics, hello, debug, output
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Print to Serial Monitor — basic Serial.begin / Serial.println pattern
// prompts: hello world serial, debug print, serial monitor output, Serial.begin, Serial.println, print variable to monitor

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);  // wait for USB serial on Leonardo/Nano 33
  Serial.println("Hello, world!");
}

void loop() {
  Serial.print("uptime ms: ");
  Serial.println(millis());
  delay(1000);
}
