// snippet: capacitive_sensor_touch
// tags: touch, capacitive, capacitivesensor, touch-lamp, toggle, sensor, foil
// boards: avr, megaavr, nano33iot, romi32u4
// library: CapacitiveSensor
// description: Touch switch with the CapacitiveSensor library (send pin -> 1M resistor -> receive pin + foil pad)
// prompts: capacitive touch Arduino, CapacitiveSensor library, touch lamp toggle, foil touch pad, touch switch LED, DIY touch sensor
// exact-include: CapacitiveSensor.h

#include <CapacitiveSensor.h>

CapacitiveSensor cs = CapacitiveSensor(4, 6);  // send pin 4, receive pin 6
const long TOUCH_THRESHOLD = 1000;
const unsigned long LOCKOUT_MS = 300;          // one toggle per touch
const int LED_PIN = 9;

unsigned long lastToggleMs = 0;
bool ledOn = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  long reading = cs.capacitiveSensor(30);      // 30 samples

  unsigned long now = millis();
  if (reading > TOUCH_THRESHOLD && now - lastToggleMs > LOCKOUT_MS) {
    lastToggleMs = now;
    ledOn = !ledOn;
    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
    Serial.print("touch ");
    Serial.println(reading);
  }
}
