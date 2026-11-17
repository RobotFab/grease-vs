// snippet: rs485_max485
// tags: rs485, max485, modbus, half-duplex, re, de, uart, industrial, serial
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Half-duplex RS-485 communication using MAX485 transceiver — DE/RE pin switching
// prompts: RS-485 communication, MAX485, Modbus serial, industrial bus, half-duplex RS485, RS485 transceiver

const int DE_RE = 2;  // tie DE and RE together to one pin

void rs485Send(const char* msg) {
  digitalWrite(DE_RE, HIGH);   // enable transmit
  Serial.print(msg);
  Serial.flush();               // wait until sent
  digitalWrite(DE_RE, LOW);    // switch back to receive
}

void setup() {
  Serial.begin(9600);
  pinMode(DE_RE, OUTPUT);
  digitalWrite(DE_RE, LOW);   // receive mode by default
}

void loop() {
  // send every 2 seconds
  static unsigned long prevMs = 0;
  if (millis() - prevMs >= 2000) {
    prevMs = millis();
    rs485Send("HELLO\n");
  }
  // echo received bytes
  if (Serial.available()) Serial.write(Serial.read());
}
