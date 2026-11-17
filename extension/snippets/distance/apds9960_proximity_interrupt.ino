// snippet: apds9960_proximity_interrupt
// tags: proximity, apds9960, sparkfun, i2c, interrupt, threshold, object-detection
// not-tags: adafruit, Adafruit_APDS9960, arduino-apds9960, gesture, color
// boards: avr, esp32, nano33iot, romi32u4, rp2040
// library: SparkFun APDS9960 RGB and Gesture Sensor
// description: Interrupt-driven proximity threshold detection with APDS9960 (SparkFun library).
//   Fires interrupt when proximity crosses PROX_INT_HIGH (object approaches).
//   ISR sets a volatile flag; main loop reads and clears the interrupt.
//   apds.clearProximityInt() MUST be called after handling or interrupts stop firing.
//   Sensor runs on 3.3V — do NOT connect directly to 5V Arduino pins without a level shifter.
// prompts: APDS9960 proximity interrupt, proximity threshold trigger, approach detection interrupt, object near interrupt APDS9960
// exact-include: Wire.h, SparkFun_APDS9960.h

#include <Wire.h>
#include <SparkFun_APDS9960.h>

#define APDS9960_INT_PIN  2
#define PROX_INT_HIGH     50   // 0–255: trigger when object closer than this
#define PROX_INT_LOW      0    // no far-edge interrupt

SparkFun_APDS9960 apds;
uint8_t proximityData = 0;
volatile bool proximityTriggered = false;

void proximityISR() {
  proximityTriggered = true;
}

void setup() {
  Serial.begin(9600);
  pinMode(APDS9960_INT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(APDS9960_INT_PIN), proximityISR, FALLING);

  Wire.begin();
  if (!apds.init()) { Serial.println("APDS9960 not found"); while (1); }
  apds.setProximityGain(PGAIN_2X);
  apds.setProximityIntLowThreshold(PROX_INT_LOW);
  apds.setProximityIntHighThreshold(PROX_INT_HIGH);
  if (!apds.enableProximitySensor(true)) { Serial.println("Proximity init failed"); while (1); }
}

void loop() {
  if (!proximityTriggered) return;
  proximityTriggered = false;

  if (!apds.readProximity(proximityData)) {
    Serial.println("Proximity read error");
  } else {
    Serial.print("proximity interrupt! level: ");
    Serial.println(proximityData);
  }

  // MUST clear interrupt register or no further interrupts fire
  apds.clearProximityInt();
}
