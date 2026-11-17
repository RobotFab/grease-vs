// snippet: apds9960_light_interrupt
// tags: ambient-light, apds9960, sparkfun, i2c, interrupt, threshold, light-sensor
// not-tags: adafruit, Adafruit_APDS9960, arduino-apds9960, gesture, proximity
// boards: avr, esp32, nano33iot, romi32u4, rp2040
// library: SparkFun APDS9960 RGB and Gesture Sensor
// description: Interrupt-driven ambient light threshold detection with APDS9960 (SparkFun library).
//   Fires when ambient light crosses LIGHT_INT_HIGH (bright) or LIGHT_INT_LOW (dark).
//   ISR sets a volatile flag; main loop reads RGBA and clears the interrupt.
//   apds.clearAmbientLightInt() MUST be called after handling or interrupts stop firing.
//   Sensor runs on 3.3V — do NOT connect directly to 5V Arduino pins without a level shifter.
// prompts: APDS9960 light interrupt, ambient light threshold, light level trigger, brightness interrupt APDS9960, dark light detection interrupt
// exact-include: Wire.h, SparkFun_APDS9960.h

#include <Wire.h>
#include <SparkFun_APDS9960.h>

#define APDS9960_INT_PIN  2
#define LIGHT_INT_HIGH    1000  // trigger above this lux
#define LIGHT_INT_LOW     10    // trigger below this lux

SparkFun_APDS9960 apds;
uint16_t ambientLight = 0, red = 0, green = 0, blue = 0;
volatile bool lightTriggered = false;

void lightISR() {
  lightTriggered = true;
}

void setup() {
  Serial.begin(9600);
  pinMode(APDS9960_INT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(APDS9960_INT_PIN), lightISR, FALLING);

  Wire.begin();
  if (!apds.init()) { Serial.println("APDS9960 not found"); while (1); }
  apds.setLightIntLowThreshold(LIGHT_INT_LOW);
  apds.setLightIntHighThreshold(LIGHT_INT_HIGH);
  if (!apds.enableLightSensor(false)) { Serial.println("Light sensor init failed"); while (1); }
  apds.setAmbientLightIntEnable(1);
}

void loop() {
  if (!lightTriggered) return;
  lightTriggered = false;

  if (!apds.readAmbientLight(ambientLight) ||
      !apds.readRedLight(red) ||
      !apds.readGreenLight(green) ||
      !apds.readBlueLight(blue)) {
    Serial.println("Light read error");
  } else {
    Serial.print("light interrupt! C: "); Serial.print(ambientLight);
    Serial.print(" R: "); Serial.print(red);
    Serial.print(" G: "); Serial.print(green);
    Serial.print(" B: "); Serial.println(blue);
  }

  // MUST clear interrupt register or no further interrupts fire
  apds.clearAmbientLightInt();
}
