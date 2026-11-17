// snippet: esp32_nimble_motor_control
// tags: ble, bluetooth, esp32, server, peripheral, motor, dc, l298n, write, characteristic, nimble, gatt, command, callback, onwrite, override, nimbleconninfo, deadman, timeout
// boards: esp32
// library: NimBLE-Arduino
// description: ESP32 BLE GATT server (NimBLE 2.x) with a writable characteristic for motor
//   control. A BLE central writes a 1-byte command: 'F'=forward, 'B'=back, 'L'=left,
//   'R'=right, 'S'=stop. Shows NIMBLE_PROPERTY::WRITE, the onWrite() callback, and an L298N.
//
//   THE CALLBACK SIGNATURES ARE THE LESSON. NimBLE-Arduino 2.x changed all of them:
//     void onWrite(NimBLECharacteristic* c, NimBLEConnInfo& info)          <- 2 arguments
//     void onConnect(NimBLEServer* s, NimBLEConnInfo& info)                <- 2 arguments
//     void onDisconnect(NimBLEServer* s, NimBLEConnInfo& info, int reason) <- 3 arguments
//   The 1.x forms — onWrite(NimBLECharacteristic*), onConnect(NimBLEServer*) — STILL COMPILE
//   under 2.x with no error and no warning. C++ treats them as new, unrelated member functions
//   that override nothing, so the base class's empty virtual runs instead and your callback is
//   never called. The device connects, the write succeeds, and the motor does nothing.
//   Write `override` on every callback and the compiler turns this silent failure into an error.
//   NOT ArduinoBLE and NOT the core's BLEDevice.h — on ESP32 the library is NimBLE-Arduino.
//
//   A BLE link to a moving robot needs a deadman. onDisconnect() covers a clean disconnect,
//   but not a central that walks out of range mid-command, so loop() also stops the motors
//   when no command has arrived recently. Non-blocking, on millis().
// prompts: ESP32 BLE motor control, NimBLE motor, BLE writable characteristic ESP32, BLE motor command ESP32, NimBLE write characteristic, control motor over BLE ESP32, NimBLE onWrite not called, NimBLE 2 callback signature, NimBLEConnInfo onWrite, NimBLE callback never fires
// exact-include: NimBLEDevice.h
// not-tags: nano33, nano33iot, nano33ble, arduinoble, samd, bleservice, blecharacteristic

#include <NimBLEDevice.h>

// L298N motor driver pins
#define IN1  25
#define IN2  26
#define IN3  27
#define IN4  14
#define ENA  32  // PWM enable A
#define ENB  33  // PWM enable B

#define SERVICE_UUID "ABCD1234-AB12-AB12-AB12-ABCDEF012345"
#define CMD_CHAR_UUID "ABCD1235-AB12-AB12-AB12-ABCDEF012345"

void motorForward()  { digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);  }
void motorBackward() { digitalWrite(IN1,LOW);  digitalWrite(IN2,HIGH); digitalWrite(IN3,LOW);  digitalWrite(IN4,HIGH); }
void motorLeft()     { digitalWrite(IN1,LOW);  digitalWrite(IN2,HIGH); digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);  }
void motorRight()    { digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);  digitalWrite(IN3,LOW);  digitalWrite(IN4,HIGH); }
void motorStop()     { digitalWrite(IN1,LOW);  digitalWrite(IN2,LOW);  digitalWrite(IN3,LOW);  digitalWrite(IN4,LOW);  }

// Last time a valid command arrived. The deadman in loop() reads it.
unsigned long lastCommandMs = 0;

class CmdCallbacks : public NimBLECharacteristicCallbacks {
  // Two arguments, and `override`. The one-argument 1.x form compiles and never fires.
  void onWrite(NimBLECharacteristic* pChar, NimBLEConnInfo& connInfo) override {
    NimBLEAttValue val = pChar->getValue();
    if (val.length() == 0) return;
    char cmd = (char)val[0];
    Serial.printf("BLE command: %c\n", cmd);
    switch (cmd) {
      case 'F': motorForward();  break;
      case 'B': motorBackward(); break;
      case 'L': motorLeft();     break;
      case 'R': motorRight();    break;
      case 'S': motorStop();     break;
      default:  Serial.println("Unknown command"); return;   // do not feed the deadman
    }
    lastCommandMs = millis();
  }
};

class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* s, NimBLEConnInfo& connInfo) override {
    lastCommandMs = millis();          // grace period before the deadman can fire
    Serial.println("Client connected");
  }
  // Three arguments in 2.x — the reason code is new.
  void onDisconnect(NimBLEServer* s, NimBLEConnInfo& connInfo, int reason) override {
    motorStop();
    Serial.printf("Client disconnected (reason %d) — motors stopped\n", reason);
    NimBLEDevice::startAdvertising();  // or the device is invisible to the next client
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  // ENA/ENB must be set OUTPUT before digitalWrite — ESP32 GPIO defaults
  // to input on boot, so digitalWrite on an unconfigured pin is a no-op.
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  digitalWrite(ENA, HIGH); digitalWrite(ENB, HIGH);
  motorStop();

  NimBLEDevice::init("ESP32-Motor");
  NimBLEServer* server = NimBLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  NimBLEService* service = server->createService(SERVICE_UUID);
  NimBLECharacteristic* pCmd = service->createCharacteristic(
    CMD_CHAR_UUID,
    NIMBLE_PROPERTY::WRITE
  );
  pCmd->setCallbacks(new CmdCallbacks());

  service->start();

  NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);   // so a scanner can filter for this robot
  adv->start();

  lastCommandMs = millis();
  Serial.println("BLE motor controller advertising");
}

// Stop if the central goes quiet — out of range does not raise onDisconnect().
const unsigned long COMMAND_TIMEOUT_MS = 1500;
bool stoppedByTimeout = false;

void loop() {
  // NOT delay() — a robot's loop must stay free to run its own failsafe.
  unsigned long now = millis();
  if (!stoppedByTimeout && now - lastCommandMs > COMMAND_TIMEOUT_MS) {
    motorStop();
    stoppedByTimeout = true;
    Serial.println("Command timeout — motors stopped");
  } else if (now - lastCommandMs <= COMMAND_TIMEOUT_MS) {
    stoppedByTimeout = false;
  }
}
