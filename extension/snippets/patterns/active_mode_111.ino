// snippet: active_mode_111
// tags: active, serial, command, response, dht
// boards: avr, esp32, megaavr, nano33iot, romi32u4, rp2040
// library: DHT
// description: Active sketch: combined node — motor speed, HC-SR04 distance, DHT22 temperature
// prompts: active, sketch, combined, motor, read_dist
// verified: compile-only
// source: kd-generated (id 111)
// exact-include: DHT.h

#include <DHT.h>

#define MOTOR_PIN  9
#define TRIG_PIN   12
#define ECHO_PIN   13
#define DHTPIN     4
#define DHTTYPE    DHT22

DHT dht(DHTPIN, DHTTYPE);

int motorSpeed = 0;
long distanceCm = 0;
float temperature = 0.0;

unsigned long prevMotorMs = 0;
unsigned long prevDistanceMs = 0;
unsigned long prevTempMs = 0;

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  dht.begin();
}

void loop() {
  unsigned long now = millis();

  // Motor speed control from potentiometer
  if (now - prevMotorMs >= 100) {
    prevMotorMs = now;
    motorSpeed = map(analogRead(A0), 0, 1023, 0, 255);
    analogWrite(MOTOR_PIN, motorSpeed);
  }

  // Distance reading — must trigger HC-SR04 before pulseIn
  if (now - prevDistanceMs >= 100) {
    prevDistanceMs = now;
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // 30ms timeout
    distanceCm = duration / 58;
  }

  // Temperature reading (DHT22 needs >= 2s between reads)
  if (now - prevTempMs >= 2000) {
    prevTempMs = now;
    float t = dht.readTemperature();
    if (!isnan(t)) {  // a failed DHT read returns NaN — keep the last good value
      temperature = t;
    }
  }

  // Handle STATUS command
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command == "STATUS") {
      Serial.print("Motor Speed: "); Serial.println(motorSpeed);
      Serial.print("Distance: "); Serial.println(distanceCm);
      Serial.print("Temperature: "); Serial.println(temperature);
    }
  }
}
