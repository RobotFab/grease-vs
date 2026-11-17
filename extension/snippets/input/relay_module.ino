// snippet: relay_module
// tags: relay, output, switch, load, ac, dc, control
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Control a single-channel relay module from a digital pin.
//   Most relay modules are active LOW (LOW = relay ON). Set ACTIVE_LOW to false for active-HIGH modules.
//   Wiring: IN → RELAY_PIN, VCC → 5V, GND → GND.
//   Example: toggle relay on Serial command 'on'/'off'.
// prompts: relay module, switch AC load, control relay digital pin, high voltage switch, turn on off relay, relay output
//
// ---- CONFIGURE ----
#define RELAY_PIN   7
#define ACTIVE_LOW  true   // most common relay modules are active LOW
// -------------------

#define RELAY_ON  (ACTIVE_LOW ? LOW  : HIGH)
#define RELAY_OFF (ACTIVE_LOW ? HIGH : LOW)

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);
  Serial.println("Relay ready. Send 'on' or 'off'.");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "on") {
      digitalWrite(RELAY_PIN, RELAY_ON);
      Serial.println("Relay ON");
    } else if (cmd == "off") {
      digitalWrite(RELAY_PIN, RELAY_OFF);
      Serial.println("Relay OFF");
    }
  }
}
