// snippet: serial_echo
// tags: serial, read, echo, input, basics, readline, parse
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Echo serial input back — read characters until newline, print full line
// prompts: serial echo, read serial input, Serial.read, parse serial command, receive serial string, read line from serial, serial readline

String inputLine = "";

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  Serial.println("Ready. Send text + Enter.");
}

void loop() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n') {
      Serial.print("echo: ");
      Serial.println(inputLine);
      inputLine = "";
    } else if (c != '\r') {
      inputLine += c;
    }
  }
}
