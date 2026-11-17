// snippet: serial_report_90
// tags: serial, report, uart, dht
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: DHT
// description: On serial command TEMP, read DHT22 pin 4, reply T:<celsius> H:<humidity>
// prompts: serial, command, dht22, <celsius>, <humidity>, declare
// verified: compile-only
// source: kd-generated (id 90)
// exact-include: DHT.h

#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
float t = 0.0, h = 0.0;
String inputBuf = "";
bool lineReady = false;

void setup() {
  Serial.begin(9600);
  dht.begin();
  Serial.println("Ready. Commands: TEMP");
}

void processCommand(const String& cmd) {
  if (cmd == "TEMP") {
    t = dht.readTemperature();
    h = dht.readHumidity();
    if (isnan(t) || isnan(h)) Serial.println("DHT read failed");
    else Serial.print("T:"); Serial.print(t); Serial.print(" H:"); Serial.println(h);
  } else {
    Serial.print("ERR unknown: "); Serial.println(cmd);
  }
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') { lineReady = true; break; }
    if (c != '\r') inputBuf += c;
  }

  if (lineReady) {
    inputBuf.trim();
    if (inputBuf.length() > 0) processCommand(inputBuf);
    inputBuf = "";
    lineReady = false;
  }
}
