// snippet: ble_notify_sensor
// tags: ble, bluetooth, peripheral, arduinoble, notify, sensor, characteristic, stream
// boards: nano33iot
// library: ArduinoBLE
// description: BLE peripheral that notifies a float value every 200ms (e.g. sensor data)
// prompts: BLE notify, Bluetooth sensor streaming, ArduinoBLE notify, BLE peripheral, stream sensor over BLE, BLE characteristic notify
// exact-include: ArduinoBLE.h
// not-tags: esp32, nimble, nimbledevice, esp32ble, bledevice

#include <ArduinoBLE.h>

BLEService sensorService("1101");
BLEFloatCharacteristic valueChar("2101", BLERead | BLENotify);

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  if (!BLE.begin()) {
    Serial.println("BLE init failed");
    while (1);
  }
  BLE.setLocalName("SensorNode");
  BLE.setAdvertisedService(sensorService);
  sensorService.addCharacteristic(valueChar);
  BLE.addService(sensorService);
  valueChar.writeValue(0.0);
  BLE.advertise();
}

void loop() {
  BLE.poll();
  unsigned long now = millis();
  if (now - prevMs < 200) return;
  prevMs = now;

  float val = analogRead(A0) * (3.3 / 1023.0);
  valueChar.writeValue(val);
  Serial.print("BLE notify: ");
  Serial.println(val);
}
