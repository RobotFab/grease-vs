// snippet: nano33iot_297
// tags: nano33iot, samd, imu, wifi, arduinoble, arduino_lsm6ds3
// boards: nano33iot
// library: ArduinoBLE, Arduino_LSM6DS3
// description: Arduino Nano 33 IoT: BLE peripheral broadcasting IMU data
// prompts: arduino, peripheral, broadcasting, include, arduinoble, arduino_lsm6ds3
// verified: compile-only
// source: kd-generated (id 297)
// exact-include: ArduinoBLE.h, Arduino_LSM6DS3.h

#include <ArduinoBLE.h>
#include <Arduino_LSM6DS3.h>

BLEService imuService("1101");
BLEFloatCharacteristic accelChar("2101", BLERead | BLENotify);

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  if (!IMU.begin()) {
    Serial.println("IMU init failed");
    while (1);
  }
  if (!BLE.begin()) {
    Serial.println("BLE init failed");
    while (1);
  }
  BLE.setLocalName("Nano33IMU");
  BLE.setAdvertisedService(imuService);
  imuService.addCharacteristic(accelChar);
  BLE.addService(imuService);
  BLE.advertise();
}

void loop() {
  BLE.poll();
  unsigned long now = millis();
  if (now - prevMs < 200) return;
  prevMs = now;

  float ax, ay, az;
  IMU.readAcceleration(ax, ay, az);  // LSM6DS3: writes g-force into the 3 float args; NOT getMotion6()
  accelChar.writeValue(ax);
  accelChar.writeValue(ay);
  accelChar.writeValue(az);
  Serial.print("BLE notify: ax="); Serial.print(ax);
  Serial.print(" ay="); Serial.print(ay);
  Serial.print(" az="); Serial.println(az);
}
