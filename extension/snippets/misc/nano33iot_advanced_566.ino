// snippet: nano33iot_advanced_566
// tags: arduino_lsm6ds3
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Arduino_LSM6DS3
// description: Free-fall detection using LSM6DS3: compute acceleration magnitude each loop
// prompts: free-fall, detection, lsm6ds3, compute, acceleration, magnitude
// verified: compile-only
// source: kd-generated (id 566)
// exact-include: Arduino_LSM6DS3.h

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // ESP32 core may not define it (rule_led_builtin)
#endif

#include <Arduino_LSM6DS3.h>

const unsigned long FREE_FALL_TIME = 50;  // milliseconds
const float FREE_FALL_THRESHOLD = 0.2;  // g

unsigned long prevMs = 0;
unsigned long freeFallStartMs = 0;
bool inFreeFall = false;
int alertToggles = 0;        // remaining LED toggles for the free-fall alert
unsigned long blinkMs = 0;
bool ledState = false;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!IMU.begin()) {
    Serial.println("IMU init failed");
    while (1);
  }
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 10) return;
  prevMs = now;

  // Non-blocking free-fall alert blink (replaces a blocking delay() loop)
  if (alertToggles > 0 && now - blinkMs >= 200) {
    blinkMs = now;
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
    alertToggles--;
  }

  if (IMU.accelerationAvailable()) {  // LSM6DS3: true when a new accel sample is ready
    float ax, ay, az;
    IMU.readAcceleration(ax, ay, az);  // LSM6DS3: writes g-force into the 3 float args; NOT getMotion6()
    float mag = sqrt(ax * ax + ay * ay + az * az);

    if (mag < FREE_FALL_THRESHOLD) {
      if (!inFreeFall) {
        freeFallStartMs = now;
        inFreeFall = true;
      }
    } else {
      inFreeFall = false;
    }

    if (inFreeFall && now - freeFallStartMs >= FREE_FALL_TIME) {
      Serial.println("FREE FALL DETECTED");
      alertToggles = 10;  // 5 blinks = 10 toggles, handled non-blockingly above
      ledState = false;
      blinkMs = now;
      inFreeFall = false;
    }
  }
}
