// snippet: vl53l0x_adafruit
// tags: distance, tof, time-of-flight, flight, vl53l0x, lidar, laser, i2c, adafruit, ranging
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: Adafruit VL53L0X
// description: Continuous ranging with Adafruit VL53L0X time-of-flight sensor
//   Adafruit_VL53L0X API: tof.begin(), then EITHER continuous mode
//   (startRangeContinuous() + isRangeComplete() + readRange()) OR one-shot
//   (rangingTest(&measure, false), reading measure.RangeMilliMeter).
//   There is NO startRanging() — that is the SparkFun VL53L1X API for a different sensor.
// prompts: VL53L0X distance sensor, time of flight ranging, ToF distance, laser range finder, lidar I2C distance
// not-tags: sparkfun, vl53l1x
// exact-include: Wire.h, Adafruit_VL53L0X.h

#include <Wire.h>
#include <Adafruit_VL53L0X.h>

Adafruit_VL53L0X tof;
unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  // Halt here. `return` from setup() does NOT stop the sketch — loop() runs anyway and polls a
  // sensor that was never started, and isRangeComplete() on a dead device just never fires, so
  // the sketch looks alive and silently reports nothing.
  if (!tof.begin()) {
    Serial.println("VL53L0X not found — check SDA/SCL, 2.8 V level shifting and XSHUT");
    while (1);
  }
  tof.startRangeContinuous();  // Adafruit API — NOT startRanging() (that is SparkFun VL53L1X)
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 100) return;
  prevMs = now;

  if (tof.isRangeComplete()) {
    uint16_t mm = tof.readRange();
    Serial.print("distance: ");
    Serial.println(mm);
  }
}
