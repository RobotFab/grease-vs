// snippet: misc_151
// tags: misc, millis, nonblocking
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: none
// description: Multiple periodic tasks at different rates: sensor sample at 10Hz, serial report at 1Hz, heartbea…
// prompts: multiple, periodic, tasks, different, rates, sensor
// verified: compile-only
// source: kd-generated (id 151)
// exact-include: Arduino.h

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

#include <Arduino.h>

// Global variables for task timing
unsigned long sensorPrevMs = 0;
unsigned long serialPrevMs = 0;
unsigned long heartbeatPrevMs = 0;

// Task intervals in milliseconds
const unsigned long SENSOR_INTERVAL = 100;  // 10Hz
const unsigned long SERIAL_INTERVAL = 1000; // 1Hz
const unsigned long HEARTBEAT_INTERVAL = 5000; // 0.2Hz

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  unsigned long now = millis();

  // Sensor sampling task
  if (now - sensorPrevMs >= SENSOR_INTERVAL) {
    sensorPrevMs = now;
    // Perform sensor reading here
    int sensorValue = analogRead(A0);
    Serial.print("sensor: "); Serial.println(sensorValue);
  }

  // Serial reporting task
  if (now - serialPrevMs >= SERIAL_INTERVAL) {
    serialPrevMs = now;
    // Perform serial reporting here
    Serial.println("reporting...");
  }

  // Heartbeat task
  if (now - heartbeatPrevMs >= HEARTBEAT_INTERVAL) {
    heartbeatPrevMs = now;
    // Perform heartbeat action here
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}
