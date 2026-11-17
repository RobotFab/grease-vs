// snippet: ble_read_characteristic
// tags: ble, bluetooth, central, read, characteristic, gatt, arduinoble, poll
// boards: nano33iot
// library: ArduinoBLE
// description: BLE central that connects to a named peripheral and reads a characteristic value every 500 ms.
// prompts: BLE read characteristic, ArduinoBLE central read, read BLE value, poll BLE characteristic, BLE GATT read central
// exact-include: ArduinoBLE.h
// not-tags: esp32, nimble, nimbledevice, esp32ble, bledevice

#include <ArduinoBLE.h>

const char TARGET_NAME[] = "SensorNode";
const char SERVICE_UUID[]    = "1101";
const char CHAR_UUID[]       = "2101";

unsigned long prevMs = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 3000);
  // Halt on init failure. `return` from setup() does NOT stop the sketch — loop() still
  // runs, against a stack that was never brought up.
  if (!BLE.begin()) {
    Serial.println("BLE init failed");
    while (1);
  }
  Serial.println("BLE central — scanning for SensorNode...");
  BLE.scanForName(TARGET_NAME);
}

void loop() {
  BLEDevice peripheral = BLE.available();
  if (!peripheral) return;

  BLE.stopScan();
  Serial.print("Connecting to "); Serial.println(peripheral.localName());

  if (!peripheral.connect()) { Serial.println("Connect failed"); BLE.scanForName(TARGET_NAME); return; }
  if (!peripheral.discoverAttributes()) { peripheral.disconnect(); BLE.scanForName(TARGET_NAME); return; }

  BLECharacteristic valueChar = peripheral.characteristic(CHAR_UUID);
  if (!valueChar || !valueChar.canRead()) {
    Serial.println("Characteristic not found or not readable");
    peripheral.disconnect();
    BLE.scanForName(TARGET_NAME);
    return;
  }

  // Poll while connected. This loop still owns the CPU for the whole session — for a robot
  // that has to keep driving, hoist the connection into a global and let loop() run once
  // per pass instead of blocking here (see ble_peripheral_nano33iot).
  while (peripheral.connected()) {
    unsigned long now = millis();
    if (now - prevMs >= 500) {
      prevMs = now;
      valueChar.read();
      // Check the length before copying — a peripheral that sent 2 bytes would otherwise
      // have 4 bytes read out of it, taking whatever follows in the buffer.
      if (valueChar.valueLength() >= sizeof(float)) {
        float val;
        memcpy(&val, valueChar.value(), sizeof(val));
        Serial.print("Value: "); Serial.println(val);
      } else {
        Serial.println("Short value — not a float");
      }
    }
  }

  Serial.println("Disconnected — rescanning");
  BLE.scanForName(TARGET_NAME);
}
