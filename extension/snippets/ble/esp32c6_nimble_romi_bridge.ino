// snippet: esp32c6_nimble_romi_bridge
// tags: esp32, esp32c6, c6, ble, bluetooth, nimble, gatt, server, peripheral, bridge, uart, serial1, romi, romi32u4, ascii, wasd, command, notify, write, telemetry, level shift
// boards: esp32
// library: NimBLE-Arduino
// description: ESP32-C6 as the radio for a Romi 32U4: a computer writes ASCII command bytes
//   (W A S D, space) over BLE, the C6 forwards each byte down Serial1 to the Romi, and the
//   Romi's replies come back as BLE notifications. FQBN esp32:esp32:esp32c6.
//
//   NIMBLE 2.x CALLBACK SIGNATURES — the single most common way this sketch silently fails.
//     void onWrite(NimBLECharacteristic* c, NimBLEConnInfo& info)          <- 2 arguments
//     void onConnect(NimBLEServer* s, NimBLEConnInfo& info)                <- 2 arguments
//     void onDisconnect(NimBLEServer* s, NimBLEConnInfo& info, int reason) <- 3 arguments
//   The NimBLE 1.x one-argument forms still COMPILE — C++ just treats them as new, unrelated
//   methods that hide nothing — but they override nothing, so the callback never fires. There
//   is no warning. Add `override` to every one of them and the compiler will catch it for you.
//   NOT ArduinoBLE and NOT the ESP32 core's BLEDevice.h — on ESP32 the library is
//     NimBLE-Arduino (NimBLEDevice.h). BLEDevice.h is the old Bluedroid stack: far larger,
//     and mixing the two APIs is the usual source of "class has no member" errors.
//
//   createService() is called on the SERVER; createCharacteristic() is called on the SERVICE.
//   Calling createCharacteristic() on the server does not exist and will not compile.
//
//   SERIAL1 PINS ARE REMAPPABLE ON ESP32, UNLIKE AVR:
//     Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
//   The 4-argument form is required — the 1-argument form uses the core's default pins, which
//   are not where you wired the Romi. On the C6 avoid GPIO8, GPIO9 and GPIO15 (strapping pins,
//   sampled at boot) and GPIO12/GPIO13 (USB Serial/JTAG). GPIO4 and GPIO5 are safe.
//
//   LEVEL SHIFTING — the C6 is NOT 5 V tolerant:
//     C6 TX (3.3 V) -> Romi RX pin 0     direct; the AVR reads 3.3 V as HIGH.
//     Romi TX pin 1 (5 V) -> C6 RX       THROUGH A DIVIDER (1k series, 2k to ground).
//     Common ground. Cross TX to RX.
//
//   NOT I2C to reach the Romi. The Romi must be the I2C master for its PCA9685 / AS5600 /
//     onboard IMU, and an AVR's TWI cannot be master and slave at once.
//   Send ONE byte per command. The Romi's parser is a switch on a char; a formatted string
//     just makes it discard bytes.
// prompts: ESP32-C6 NimBLE Romi bridge, ESP32 bluetooth to serial Romi, ESP32-C6 BLE UART command, NimBLE forward bytes to Serial1, ESP32 C6 BLE relay WASD, ESP32-C6 bluetooth robot control, NimBLE onWrite two arguments, ESP32-C6 Serial1 pins, ESP32 BLE notify telemetry robot, esp32c6 bluetooth romi 32u4
// not-tags: nano33, nano33iot, nano33ble, arduinoble, samd, romi32u4, avr, leonardo
// exact-include: NimBLEDevice.h

#include <NimBLEDevice.h>

// Nordic UART Service UUIDs — most BLE terminal apps already know them.
#define SERVICE_UUID   "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define RX_CHAR_UUID   "6e400002-b5a3-f393-e0a9-e50e24dcca9e"  // computer -> C6 (WRITE)
#define TX_CHAR_UUID   "6e400003-b5a3-f393-e0a9-e50e24dcca9e"  // C6 -> computer (NOTIFY)

// ── Serial1 wiring to the Romi ────────────────────────────────────────
const int ROMI_RX_PIN = 4;   // C6 receives here  <- Romi TX pin 1, THROUGH A DIVIDER
const int ROMI_TX_PIN = 5;   // C6 transmits here -> Romi RX pin 0, direct

NimBLECharacteristic* txChar = nullptr;
bool connected = false;

// ── Server callbacks — note the argument counts ───────────────────────
class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
    connected = true;
  }
  void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
    connected = false;
    NimBLEDevice::startAdvertising();   // or the C6 is invisible after the first client leaves
  }
};

// ── Characteristic callbacks — BLE write becomes UART bytes ───────────
class RxCallbacks : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
    NimBLEAttValue value = pCharacteristic->getValue();
    for (size_t i = 0; i < value.length(); i++) {
      char c = (char)value[i];
      // Forward only what the Romi's switch understands. Dropping the rest here
      // keeps line endings from a phone keyboard out of the robot's parser.
      if (c == 'W' || c == 'A' || c == 'S' || c == 'D' ||
          c == 'w' || c == 'a' || c == 's' || c == 'd' || c == ' ') {
        Serial1.write(c);
      }
    }
  }
};

void setup() {
  Serial.begin(115200);                                       // USB, for debugging
  Serial1.begin(115200, SERIAL_8N1, ROMI_RX_PIN, ROMI_TX_PIN); // the Romi link

  NimBLEDevice::init("Romi-C6");
  NimBLEDevice::setPower(3);            // dBm

  NimBLEServer* server = NimBLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  NimBLEService* service = server->createService(SERVICE_UUID);

  // createCharacteristic() on the SERVICE, not on the server.
  NimBLECharacteristic* rxChar = service->createCharacteristic(
      RX_CHAR_UUID, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
  rxChar->setCallbacks(new RxCallbacks());

  txChar = service->createCharacteristic(
      TX_CHAR_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);

  service->start();

  NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  adv->start();

  Serial.println("Advertising as Romi-C6");
}

void loop() {
  // ── Romi -> BLE. Collect a line, notify it, never block. ───────────
  static char line[32];
  static uint8_t n = 0;

  while (Serial1.available()) {
    char c = Serial1.read();
    if (c == '\n' || c == '\r' || n >= sizeof(line) - 1) {
      if (n > 0) {
        line[n] = '\0';
        if (connected && txChar) {
          txChar->setValue((uint8_t*)line, n);
          txChar->notify();
        }
        n = 0;
      }
    } else {
      line[n++] = c;
    }
  }

  // No delay() here or anywhere. A notify on every loop would flood the connection
  // interval and the client would drop packets — so notify only when the Romi has
  // actually said something, which is what the line assembly above guarantees.
}
