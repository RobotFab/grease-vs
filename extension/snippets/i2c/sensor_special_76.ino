// snippet: sensor_special_76
// tags: sensor, i2c, load-cell
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Sound sensor on A0
// prompts: sound, sensor, detect, reading, previous, reading
// verified: compile-only
// source: kd-generated (id 76)

int prevSound = 0;
int clapCount = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  static unsigned long prevMs = 0;
  unsigned long now = millis();
  if (now - prevMs >= 100) {
    prevMs = now;
    int currentSound = analogRead(A0);
    if (currentSound > 800 && prevSound < 400) {
      clapCount++;
      Serial.print("CLAP! Total: "); Serial.println(clapCount);
    }
    prevSound = currentSound;
  }
}
