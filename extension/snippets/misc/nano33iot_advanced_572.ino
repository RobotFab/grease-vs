// snippet: nano33iot_advanced_572
// tags: arduinoble
// boards: nano33iot
// library: ArduinoBLE
// description: BLE-controlled LED: advertise a writable BLE characteristic
// prompts: ble-controlled, advertise, writable, characteristic, central, writes
// verified: compile-only
// source: kd-generated (id 572)
// exact-include: ArduinoBLE.h

#include <ArduinoBLE.h>

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEByteCharacteristic switchChar("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  if (!BLE.begin()) {
    Serial.println("BLE init failed");
    while (1);
  }

  BLE.setLocalName("Nano33-LED");
  BLE.setAdvertisedService(ledService);
  ledService.addCharacteristic(switchChar);
  BLE.addService(ledService);
  switchChar.writeValue(0);
  BLE.advertise();
  Serial.println("BLE advertising");
}

void loop() {
  BLEDevice central = BLE.central();
  if (central) {
    while (central.connected()) {
      if (switchChar.written()) {
        if (switchChar.value() == 1) {
          digitalWrite(LED_BUILTIN, HIGH);
          Serial.println("ON");
        } else {
          digitalWrite(LED_BUILTIN, LOW);
          Serial.println("OFF");
        }
      }
    }
  }
}
