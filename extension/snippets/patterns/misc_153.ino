// snippet: misc_153
// tags: misc, millis, nonblocking
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Exponential moving average: declare float filteredValue=0
// prompts: exponential, moving, average, float, filteredvalue=0, float
// verified: compile-only
// source: kd-generated (id 153)

float filteredValue = 0.0;
float alpha = 0.1;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  int raw = analogRead(A0);
  filteredValue = alpha * raw + (1.0 - alpha) * filteredValue;
  Serial.print("filteredValue: ");
  Serial.println(filteredValue);
}
