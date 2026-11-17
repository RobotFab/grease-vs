// snippet: mq2_gas_sensor
// tags: gas, mq2, smoke, lpg, methane, analog, air-quality, sensor, threshold
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Read MQ-2 smoke/gas analog sensor — raw value + threshold alert (requires 20s warm-up)
// prompts: MQ-2 gas sensor, smoke detector, LPG methane detection, air quality alarm, gas threshold alert, smoke sensing

const int MQ2_PIN   = A0;
const int BUZZ_PIN  = 7;
const int THRESHOLD = 400;   // adjust for your environment

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BUZZ_PIN, OUTPUT);
  Serial.println("Warming up MQ-2... 20 sec");
  delay(20000);
  Serial.println("Ready");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 200) return;
  prevMs = now;

  int raw = analogRead(MQ2_PIN);
  Serial.print("MQ2: "); Serial.println(raw);
  digitalWrite(BUZZ_PIN, raw > THRESHOLD ? HIGH : LOW);
}
