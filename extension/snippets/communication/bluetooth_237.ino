// snippet: bluetooth_237
// tags: bluetooth, hc05, serial, wireless, softwareserial, dht, servo
// boards: avr, megaavr
// not-tags: romi32u4
// library: SoftwareSerial, DHT, Servo
// description: HC-05 on SoftwareSerial(2,3): receive SERVO <angle>, move servo pin 9
// prompts: hc-05, softwareserial(2, receive, servo, <angle>, servo
// verified: compile-only
// source: kd-generated (id 237)
// exact-include: SoftwareSerial.h, DHT.h, Servo.h

#include <SoftwareSerial.h>
#include <DHT.h>
#include <Servo.h>

#define BT_RX_PIN  2
#define BT_TX_PIN  3
#define BT_BAUD    9600
#define DHTPIN     4
#define DHTTYPE    DHT22
#define SERVO_PIN  9

SoftwareSerial bt(BT_RX_PIN, BT_TX_PIN);
DHT dht(DHTPIN, DHTTYPE);
Servo myServo;

void setup() {
  Serial.begin(115200);
  bt.begin(BT_BAUD);
  dht.begin();
  myServo.attach(SERVO_PIN);
}

void loop() {
  static unsigned long servoUpdateMs = 0;
  static unsigned long tempUpdateMs = 0;
  const unsigned long UPDATE_INTERVAL = 1000;  // 1 second

  // Handle incoming servo angle via Bluetooth
  if (bt.available()) {
    String command = bt.readStringUntil('\n');
    if (command.startsWith("SERVO ")) {
      int angle = command.substring(6).toInt();
      myServo.write(angle);
    }
  }

  // Send temperature over Bluetooth
  unsigned long now = millis();
  if (now - tempUpdateMs >= UPDATE_INTERVAL) {
    tempUpdateMs = now;
    float temp = dht.readTemperature();
    if (!isnan(temp)) {
      bt.print("temp: ");
      bt.println(temp);
    }
  }
}
