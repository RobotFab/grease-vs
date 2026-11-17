// snippet: active_mode_102
// tags: active, serial, command, response
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Active sketch: TOGGLE command flips LED on pin 13
// prompts: active, sketch, toggle, command, flips, reply
// verified: compile-only
// source: kd-generated (id 102)

bool ledState = false;
unsigned long blinkInterval = 0;
unsigned long prevBlinkMs = 0;

void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "TOGGLE") {
      ledState = !ledState;
      digitalWrite(13, ledState);
      Serial.print("LED:");
      Serial.println(ledState ? "ON" : "OFF");
    } else if (command.startsWith("BLINK")) {
      blinkInterval = command.substring(6).toInt();
      Serial.print("BLINK:");
      Serial.println(blinkInterval);
    }
  }

  unsigned long now = millis();
  if (blinkInterval > 0 && now - prevBlinkMs >= blinkInterval) {
    prevBlinkMs = now;
    ledState = !ledState;
    digitalWrite(13, ledState);
  }
}
