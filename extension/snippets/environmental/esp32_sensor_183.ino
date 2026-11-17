// snippet: esp32_sensor_183
// tags: esp32, sensor, apds9960, sparkfun, gesture, millis, i2c
// not-tags: adafruit, Adafruit_APDS9960
// boards: avr, esp32, nano33iot, romi32u4, rp2040
// library: SparkFun APDS9960 RGB and Gesture Sensor
// description: ESP32: interrupt-driven APDS9960 gesture detection with SparkFun library.
//   SparkFun gesture constants: DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT, DIR_NEAR, DIR_FAR.
//   NOT Adafruit_APDS9960 — that library uses APDS9960_UP/DOWN/LEFT/RIGHT (different API).
// prompts: esp32 apds9960 gesture, ESP32 gesture sensor, ESP32 APDS9960 SparkFun, gesture detection ESP32
// verified: compile-only
// source: kd-generated (id 183)
// exact-include: Wire.h, SparkFun_APDS9960.h

#include <Wire.h>
#include <SparkFun_APDS9960.h>

#define APDS9960_INT_PIN 2

SparkFun_APDS9960 apds;
uint8_t gesture = 0;
volatile bool gestureReady = false;

void gestureISR() {
  gestureReady = true;
}

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  pinMode(APDS9960_INT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(APDS9960_INT_PIN), gestureISR, FALLING);

  Wire.begin();
  if (!apds.init()) { Serial.println("APDS9960 not found"); while (1); }
  if (!apds.enableGestureSensor(true)) { Serial.println("Gesture init failed"); while (1); }
}

void loop() {
  if (!gestureReady) return;

  detachInterrupt(digitalPinToInterrupt(APDS9960_INT_PIN));
  gestureReady = false;

  if (apds.isGestureAvailable()) {
    gesture = apds.readGesture();
    // SparkFun constants: DIR_UP/DOWN/LEFT/RIGHT/NEAR/FAR — NOT APDS9960_UP etc.
    if (gesture == DIR_UP)    Serial.println("UP");
    if (gesture == DIR_DOWN)  Serial.println("DOWN");
    if (gesture == DIR_LEFT)  Serial.println("LEFT");
    if (gesture == DIR_RIGHT) Serial.println("RIGHT");
  }

  attachInterrupt(digitalPinToInterrupt(APDS9960_INT_PIN), gestureISR, FALLING);
}
