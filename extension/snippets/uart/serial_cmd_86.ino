// snippet: serial_cmd_86
// tags: serial, command, parser, uart
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: CALIBRATE command: on receipt, sample A0 200 times, compute mean and stddev
// prompts: calibrate, command, receipt, sample, times, compute
// verified: compile-only
// source: kd-generated (id 86)

float calMean = 0.0;
float calStddev = 0.0;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

void loop() {
  if (!Serial.available()) return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();
  if (cmd.length() == 0) return;

  if (cmd == "CALIBRATE") {
    // Use long (32-bit) — int is 16-bit on AVR and 200×1023 = 204600 overflows it.
    long sum = 0;
    for (int i = 0; i < 200; i++) {
      sum += analogRead(A0);
      delay(1);  // Small delay to avoid overwhelming the ADC
    }
    calMean = (float)sum / 200.0;
    float sumOfSquares = 0.0;
    for (int i = 0; i < 200; i++) {
      int val = analogRead(A0);
      sumOfSquares += (val - calMean) * (val - calMean);
    }
    calStddev = sqrt(sumOfSquares / 200.0);
    Serial.print("MEAN:"); Serial.print(calMean); Serial.print(" STDDEV:"); Serial.println(calStddev);
  }
}
