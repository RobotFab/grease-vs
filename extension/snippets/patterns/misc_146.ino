// snippet: misc_146
// tags: misc, millis, nonblocking
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Ring buffer: declare int samples[16], int head=0, int count=0 globally
// prompts: buffer, samples[16], head=0, count=0, globally, reading
// verified: compile-only
// source: kd-generated (id 146)

int samples[16];
int head = 0;
int count = 0;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop() {
  static unsigned long lastReadMs = 0;
  if (millis() - lastReadMs >= 100) {
    lastReadMs = millis();
    int reading = analogRead(A0);
    pushSample(reading);
  }

  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "DUMP") {
      dumpSamples();
    }
  }
}

void pushSample(int sample) {
  if (count < 16) {
    samples[head] = sample;
    head = (head + 1) % 16;
    count++;
  }
}

void dumpSamples() {
  Serial.println("Dumping samples:");
  for (int i = 0; i < count; i++) {
    Serial.print("Sample "); Serial.print(i); Serial.print(": "); Serial.println(samples[i]);
  }
}
