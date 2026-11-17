// snippet: nano33ble_imu_ble
// tags: ble, bluetooth, nano33ble, imu, lsm9ds1, accelerometer, gyroscope, notify, stream
// boards: nano33ble
// library: ArduinoBLE, Arduino_LSM9DS1
// description: Nano 33 BLE streams IMU (accelerometer + gyroscope) data over BLE notify at 50 Hz.
//   Uses LSM9DS1 — NOT LSM6DS3 (that is the Nano 33 IoT sensor).
//   Give custom data a custom 128-bit UUID. Short 16-bit UUIDs like "1809"/"2A1C" are ASSIGNED
//   by the Bluetooth SIG — 0x1809 is Health Thermometer and 0x2A1C is Temperature Measurement,
//   which has a defined IEEE-11073 float layout. A generic BLE client will decode six packed
//   int16s as a temperature reading and show nonsense. Generate a random 128-bit UUID once and
//   use it for the life of the product.
//   Only writeValue() while a central is actually connected. Notifying into an empty connection
//   is wasted work at 50 Hz, and BLE.poll() must still be called on every pass to service the
//   stack — this is the non-blocking form. `while (central.connected())` is the blocking form
//   and it locks the sketch out of everything else for the whole session.
// prompts: Nano 33 BLE IMU, BLE IMU stream, LSM9DS1 BLE, accelerometer BLE Nano 33, IMU Bluetooth Arduino, stream IMU over BLE
// exact-include: ArduinoBLE.h, Arduino_LSM9DS1.h
// not-tags: esp32, nimble, nimbledevice, esp32ble, bledevice

#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>  // NOT Arduino_LSM6DS3 — that is the Nano 33 IoT library

// Custom 128-bit UUIDs — this data is not a SIG-assigned profile.
BLEService imuService("19b10000-e8f2-537e-4f6c-d104768a1214");
// 12 bytes: ax ay az gx gy gz (each int16, scaled ×100)
BLECharacteristic imuChar("19b10001-e8f2-537e-4f6c-d104768a1214", BLERead | BLENotify, 12);

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  // Halt on either failure. `return` from setup() does NOT stop the sketch — loop() would run
  // on against a radio that was never brought up, quietly doing nothing at 50 Hz.
  if (!IMU.begin()) { Serial.println("IMU init failed"); while (1); }
  if (!BLE.begin()) { Serial.println("BLE init failed"); while (1); }

  BLE.setLocalName("Nano33BLE-IMU");
  BLE.setAdvertisedService(imuService);
  imuService.addCharacteristic(imuChar);
  BLE.addService(imuService);
  BLE.advertise();
  Serial.println("BLE IMU advertising");
}

void loop() {
  BLE.poll();                     // service the stack every pass, connected or not

  if (!BLE.connected()) return;   // nothing to notify to

  unsigned long now = millis();
  if (now - prevMs < 20) return;  // 50 Hz
  prevMs = now;

  float ax, ay, az, gx, gy, gz;
  if (!IMU.accelerationAvailable() || !IMU.gyroscopeAvailable()) return;
  IMU.readAcceleration(ax, ay, az);
  IMU.readGyroscope(gx, gy, gz);

  int16_t buf[6] = {
    (int16_t)(ax * 100), (int16_t)(ay * 100), (int16_t)(az * 100),
    (int16_t)(gx * 100), (int16_t)(gy * 100), (int16_t)(gz * 100)
  };
  imuChar.writeValue((uint8_t*)buf, sizeof(buf));
}
