// snippet: apds9960_proximity
// tags: proximity, apds9960, sparkfun, i2c, distance, object-detection
// not-tags: adafruit, Adafruit_APDS9960, arduino-apds9960, gesture, color
// boards: avr, esp32, nano33iot, romi32u4, rp2040
// library: SparkFun APDS9960 RGB and Gesture Sensor
// description: Poll proximity distance (0–255) from APDS9960 using the SparkFun library.
//   Init: apds.init(), apds.setProximityGain(PGAIN_2X), apds.enableProximitySensor(false).
//   Read: apds.readProximity(val) — by reference, uint8_t. 0 = far, 255 = very close.
//   Sensor runs on 3.3V — do NOT connect directly to 5V Arduino pins without a level shifter.
// prompts: APDS9960 proximity, proximity sensor APDS9960, object detection proximity, close distance detection, SparkFun proximity sensor
// exact-include: Wire.h, SparkFun_APDS9960.h

#include <Wire.h>
#include <SparkFun_APDS9960.h>

SparkFun_APDS9960 apds;
uint8_t proximityData = 0;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (!apds.init()) { Serial.println("APDS9960 not found"); while (1); }
  apds.setProximityGain(PGAIN_2X);
  if (!apds.enableProximitySensor(false)) { Serial.println("Proximity init failed"); while (1); }
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 250) return;
  prevMs = now;

  if (!apds.readProximity(proximityData)) {
    Serial.println("Proximity read error");
    return;
  }
  Serial.print("proximity: ");
  Serial.println(proximityData);  // 0 = far, 255 = very close
}
