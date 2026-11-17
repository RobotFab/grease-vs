// snippet: ble_peripheral_nano33iot
// tags: ble, bluetooth, peripheral, arduinoble, nano33iot, characteristic, gatt, central
// boards: nano33iot
// library: ArduinoBLE
// description: BLE peripheral with a writable characteristic on Nano 33 IoT (ArduinoBLE).
//   Poll the radio from loop() — do NOT sit in `while (central.connected())`. That inner loop owns
//   the sketch for the whole BLE session: no sensor is read, no motor is updated, no other state
//   advances until the phone disconnects. The non-blocking form is BLE.poll() every pass plus an
//   early return when nothing is connected, which leaves loop() free for the rest of the robot.
//   Give custom data a custom 128-bit UUID. Short 16-bit UUIDs are ASSIGNED by the Bluetooth SIG
//   and a generic client will decode your bytes against the wrong profile.
// prompts: BLE peripheral Nano 33 IoT, ArduinoBLE, BLE GATT, writable characteristic, Bluetooth Nano 33, BLE service
// exact-include: ArduinoBLE.h
// not-tags: esp32, nimble, nimbledevice, esp32ble, bledevice

#include <ArduinoBLE.h>

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEByteCharacteristic switchChar("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  // Halt on failure. `return` from setup() does NOT stop the sketch — loop() would run on
  // against a radio that was never brought up, advertising nothing, forever.
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
  BLE.poll();                     // service the stack every pass, connected or not

  if (!BLE.connected()) return;   // nobody to serve; loop() stays free for the rest of the robot

  if (switchChar.written()) {     // true once per central write
    digitalWrite(LED_BUILTIN, switchChar.value() ? HIGH : LOW);
  }
}
