// snippet: sensor_distance_55
// tags: distance, hcsr04, ultrasonic, trigger, echo, newping
// boards: avr, esp32, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: NewPing
// description: Three HC-SR04 sensors: left(12/13), center(10/11), right(8/9)
// prompts: three, hc-sr04, sensors, left(12/13), center(10/11), right(8/9)
// verified: compile-only
// source: kd-generated (id 55)
// exact-include: NewPing.h

#include <NewPing.h>

#define TRIG_L 12
#define ECHO_L 13
#define TRIG_C 10
#define ECHO_C 11
#define TRIG_R 8
#define ECHO_R 9
#define MAX_DIST 200

NewPing sonarL(TRIG_L, ECHO_L, MAX_DIST);
NewPing sonarC(TRIG_C, ECHO_C, MAX_DIST);
NewPing sonarR(TRIG_R, ECHO_R, MAX_DIST);

float distL = 0.0;
float distC = 0.0;
float distR = 0.0;

unsigned long prevMs = 0;
const unsigned long INTERVAL = 200;

void setup() {
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= INTERVAL) {
    prevMs = now;

    // Trigger left sensor
    distL = sonarL.ping_cm();  // NewPing: distance in cm (blocking ~10us-4ms)
    if (distL == 0) distL = -1.0; // out of range

    // Trigger center sensor
    distC = sonarC.ping_cm();  // NewPing: distance in cm (blocking ~10us-4ms)
    if (distC == 0) distC = -1.0; // out of range

    // Trigger right sensor
    distR = sonarR.ping_cm();  // NewPing: distance in cm (blocking ~10us-4ms)
    if (distR == 0) distR = -1.0; // out of range

    // Print distances as CSV
    Serial.print(distL); Serial.print(",");
    Serial.print(distC); Serial.print(",");
    Serial.println(distR);
  }
}
