// snippet: sensor_hall_584
// tags: hall, magnetic, sensor, fan, speed, rpm, pulse, interrupt
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Fan speed monitor: count Hall pulses on pin 2 (2 pulses per revolution)
// prompts: speed, monitor, count, pulses, pulses, revolution)
// verified: compile-only
// source: kd-generated (id 584)

const int HALL_PIN = 2;
const unsigned long INTERVAL = 1000;  // 1 second
const int PULSES_PER_REV = 2;

volatile unsigned long pulseCount = 0;
unsigned long lastPrintTime = 0;
unsigned long rpm = 0;

// ISR: count one pulse per falling edge
void hallISR() {
  pulseCount++;
}

void setup() {
  pinMode(HALL_PIN, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), hallISR, FALLING);
}

void loop() {
  unsigned long now = millis();

  // Calculate RPM every second
  if (now - lastPrintTime >= INTERVAL) {
    lastPrintTime = now;

    noInterrupts();
    unsigned long count = pulseCount;
    pulseCount = 0;
    interrupts();

    // RPM = (pulses / pulses-per-rev) * (60 seconds / interval in seconds)
    rpm = (count * 60) / PULSES_PER_REV;
    Serial.print("RPM: "); Serial.println(rpm);

    // Check under-speed alarm
    if (rpm < 500) {
      digitalWrite(13, HIGH);
    } else {
      digitalWrite(13, LOW);
    }
  }
}
