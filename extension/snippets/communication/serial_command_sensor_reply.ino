// snippet: serial_command_sensor_reply
// tags: serial, command, sensor, reply, response, potentiometer, analog, request, query, readStringUntil
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Serial command → sensor read → formatted reply pattern.
//   Host sends a command string (e.g. READ, PING, STATUS); the sketch reads a sensor
//   and sends the value back. Uses Serial.readStringUntil('\n') for clean line framing.
//   Potentiometer on A0 — swap analogRead(A0) for any sensor that returns a number.
//   This is the request/response pattern, NOT periodic polling.
// prompts: serial command read sensor reply, on command read potentiometer, serial request response sensor, READ command send value, query sensor over serial, command triggered analog read, serial poll sensor on request, PING reply sensor value, serial sensor query response, on serial command reply with reading

const int POT_PIN = A0;

void setup() {
  Serial.begin(9600);
  pinMode(POT_PIN, INPUT);
  Serial.println("Ready. Commands: READ  VOLT  PING");
}

void loop() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  if (cmd.length() == 0) return;

  if (cmd == "READ") {
    int raw = analogRead(POT_PIN);
    Serial.print("A0:"); Serial.println(raw);

  } else if (cmd == "VOLT") {
    int raw = analogRead(POT_PIN);
    float volt = raw * (5.0 / 1023.0);
    Serial.print("VOLT:"); Serial.println(volt, 2);

  } else if (cmd == "PING") {
    Serial.println("PONG");

  } else {
    Serial.print("ERR unknown: "); Serial.println(cmd);
  }
}
