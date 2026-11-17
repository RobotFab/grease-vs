// snippet: ble_central_scan
// tags: ble, bluetooth, central, scan, discover, arduinoble, peripheral, nearby
// boards: nano33iot
// library: ArduinoBLE
// description: BLE central role — scan for nearby peripherals and print name, address, and RSSI.
// prompts: BLE central scan, ArduinoBLE scan, discover BLE devices, BLE scanner Arduino, scan Bluetooth peripherals, BLE central role
// exact-include: ArduinoBLE.h
// not-tags: esp32, nimble, nimbledevice, esp32ble, bledevice

#include <ArduinoBLE.h>

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);

  if (!BLE.begin()) {
    Serial.println("BLE init failed");
    while (1);
  }
  Serial.println("BLE central — scanning...");
  BLE.scan();
}

void loop() {
  BLEDevice peripheral = BLE.available();
  if (peripheral) {
    Serial.print("Found: ");
    Serial.print(peripheral.address());
    Serial.print("  RSSI: "); Serial.print(peripheral.rssi());
    if (peripheral.hasLocalName()) {
      Serial.print("  Name: "); Serial.print(peripheral.localName());
    }
    if (peripheral.hasAdvertisedServiceUuid()) {
      Serial.print("  Service: "); Serial.print(peripheral.advertisedServiceUuid());
    }
    Serial.println();
  }
}
