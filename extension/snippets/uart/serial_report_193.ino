// snippet: serial_report_193
// tags: serial, report, uart
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: On ANALOG command, read all six analog pins A0–A5, print as A0:<v>,A1:<v>,A2:<v>,A3:<v>,A4:<v>,A5…
// prompts: analog, command, analog, a0–a5, declare, analogvals[6]
// verified: compile-only
// source: kd-generated (id 193)

int analogVals[6];

void setup() {
  Serial.begin(9600);
}

void loop() {
  static unsigned long t = 0;
  if (millis() - t >= 1000) {  // Check every second
    t = millis();
    for (int i = 0; i < 6; i++) {
      analogVals[i] = analogRead(i);
    }
  }

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "ANALOG") {
      Serial.print("A0:"); Serial.print(analogVals[0]); Serial.print(",");
      Serial.print("A1:"); Serial.print(analogVals[1]); Serial.print(",");
      Serial.print("A2:"); Serial.print(analogVals[2]); Serial.print(",");
      Serial.print("A3:"); Serial.print(analogVals[3]); Serial.print(",");
      Serial.print("A4:"); Serial.print(analogVals[4]); Serial.print(",");
      Serial.print("A5:"); Serial.println(analogVals[5]);
    }
  }
}
