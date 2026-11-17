// snippet: apds9960_gesture
// tags: gesture, apds9960, sparkfun, i2c, swipe, interrupt, direction
// not-tags: adafruit, Adafruit_APDS9960, arduino-apds9960
// boards: avr, esp32, nano33iot, romi32u4, rp2040
// library: SparkFun APDS9960 RGB and Gesture Sensor
// description: Interrupt-driven gesture detection (UP/DOWN/LEFT/RIGHT/NEAR/FAR) with SparkFun APDS9960.
//   SparkFun gesture API: enableGestureSensor(true), isGestureAvailable(), readGesture().
//   Returns DIR_UP/DIR_DOWN/DIR_LEFT/DIR_RIGHT/DIR_NEAR/DIR_FAR — NOT APDS9960_UP/DOWN (that is Adafruit).
//   Requires interrupt wire from APDS9960 INT pin to Arduino pin 2 (or any interrupt-capable pin).
//   Sensor runs on 3.3V — do NOT connect directly to 5V Arduino pins without a level shifter.
// prompts: APDS9960 gesture, swipe detection, gesture sensor, swipe left right up down, NEAR FAR gesture, APDS9960 interrupt gesture, SparkFun gesture sensor
// exact-include: Wire.h, SparkFun_APDS9960.h

#include <Wire.h>
#include <SparkFun_APDS9960.h>

#define APDS9960_INT_PIN 2

SparkFun_APDS9960 apds;
volatile bool gestureReady = false;

void gestureISR() {
  gestureReady = true;
}

void setup() {
  Serial.begin(9600);
  pinMode(APDS9960_INT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(APDS9960_INT_PIN), gestureISR, FALLING);

  Wire.begin();
  if (!apds.init()) { Serial.println("APDS9960 not found"); while (1); }
  if (!apds.enableGestureSensor(true)) { Serial.println("Gesture init failed"); while (1); }
  Serial.println("APDS9960 gesture ready");
}

void loop() {
  if (!gestureReady) return;

  detachInterrupt(digitalPinToInterrupt(APDS9960_INT_PIN));
  gestureReady = false;

  if (apds.isGestureAvailable()) {
    switch (apds.readGesture()) {
      // SparkFun constants: DIR_UP/DOWN/LEFT/RIGHT/NEAR/FAR — NOT APDS9960_UP etc.
      case DIR_UP:    Serial.println("UP");    break;
      case DIR_DOWN:  Serial.println("DOWN");  break;
      case DIR_LEFT:  Serial.println("LEFT");  break;
      case DIR_RIGHT: Serial.println("RIGHT"); break;
      case DIR_NEAR:  Serial.println("NEAR");  break;
      case DIR_FAR:   Serial.println("FAR");   break;
      default:        break;
    }
  }

  attachInterrupt(digitalPinToInterrupt(APDS9960_INT_PIN), gestureISR, FALLING);
}
