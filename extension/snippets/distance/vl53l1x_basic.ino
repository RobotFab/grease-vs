// snippet: vl53l1x_basic
// tags: distance, vl53l1x, lidar, tof, time-of-flight, i2c, sparkfun, 4m
// boards: TBD
// library: SparkFun VL53L1X 4m Laser Distance Sensor
// description: Short/long range ToF distance from VL53L1X using SparkFun library
// prompts: VL53L1X distance sensor, 4 meter time of flight, long range ToF, SparkFun lidar, laser distance 4m
// not-tags: adafruit, vl53l0x
// exact-include: Wire.h, SparkFun_VL53L1X.h

#include <Wire.h>
#include <SparkFun_VL53L1X.h>

SFEVL53L1X distSensor;
unsigned long prevMs = 0;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  if (distSensor.begin() != 0) { Serial.println("VL53L1X not found"); while (1); }
  distSensor.startRanging();
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs < 50) return;
  prevMs = now;

  if (distSensor.checkForDataReady()) {
    int dist = distSensor.getDistance();
    distSensor.clearInterrupt();
    Serial.print("distance: "); Serial.print(dist); Serial.println(" mm");
  }
}
