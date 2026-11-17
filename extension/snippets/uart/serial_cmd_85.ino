// snippet: serial_cmd_85
// tags: serial, command, parser, uart
// boards: avr, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: STATUS command: reply with comma-separated states of pins 4/5/6/7 (digital) and A0/A1 (analog)
// prompts: status, command, comma-separated, states, 4/5/6/7, (digital)
// verified: compile-only
// source: kd-generated (id 85)

void setup() {
  Serial.begin(9600);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
}

void loop() {
  static unsigned long t = 0;
  if (millis() - t >= 1000) {
    t = millis();
    int d4 = digitalRead(4);
    int d5 = digitalRead(5);
    int d6 = digitalRead(6);
    int d7 = digitalRead(7);
    int a0 = analogRead(A0);
    int a1 = analogRead(A1);
    Serial.print("D4:"); Serial.print(d4); Serial.print(",D5:"); Serial.print(d5); 
    Serial.print(",D6:"); Serial.print(d6); Serial.print(",D7:"); Serial.print(d7); 
    Serial.print(",A0:"); Serial.print(a0); Serial.print(",A1:"); Serial.println(a1);
  }
}
