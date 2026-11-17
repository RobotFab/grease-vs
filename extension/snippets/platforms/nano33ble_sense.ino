// snippet: nano33ble_sense
// tags: nano33ble, nano33, ble-sense, lsm9ds1, hts221, apds9960, mp34dt05, platform
// boards: nano33ble
// library: Arduino_LSM9DS1, Arduino_HTS221, Arduino_APDS9960
// description: Nano 33 BLE Sense onboard sensors — IMU, temp/humidity, gesture
// prompts: Nano 33 BLE Sense, onboard sensors Nano 33, LSM9DS1 HTS221 APDS9960, gesture sensor platform, BLE sense starter
// exact-include: Arduino_LSM9DS1.h, Arduino_HTS221.h, Arduino_APDS9960.h

#include <Arduino_LSM9DS1.h>
#include <Arduino_HTS221.h>
#include <Arduino_APDS9960.h>

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  if (!IMU.begin())   { Serial.println("LSM9DS1 failed"); while (1); }
  if (!HTS.begin())   { Serial.println("HTS221 failed");  while (1); }
  if (!APDS.begin())  { Serial.println("APDS9960 failed"); while (1); }
  APDS.setGestureSensitivity(80);
}

void loop() {
  // gesture
  if (APDS.gestureAvailable()) {
    int g = APDS.readGesture();
    if (g == GESTURE_UP)    Serial.println("UP");
    if (g == GESTURE_DOWN)  Serial.println("DOWN");
    if (g == GESTURE_LEFT)  Serial.println("LEFT");
    if (g == GESTURE_RIGHT) Serial.println("RIGHT");
  }

  unsigned long now = millis();
  if (now - prevMs < 1000) return;
  prevMs = now;

  Serial.print("T: "); Serial.print(HTS.readTemperature(), 1);
  Serial.print("°C  H: "); Serial.print(HTS.readHumidity(), 1); Serial.println("%");

  if (IMU.accelerationAvailable()) {
    float ax, ay, az;
    IMU.readAcceleration(ax, ay, az);
    Serial.print("ax: "); Serial.print(ax, 2);
    Serial.print(" ay: "); Serial.print(ay, 2);
    Serial.print(" az: "); Serial.println(az, 2);
  }
}
