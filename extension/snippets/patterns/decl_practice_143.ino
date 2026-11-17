// snippet: decl_practice_143
// tags: globals, declare, millis, nonblocking, dht
// boards: avr, esp32, megaavr, nano33iot, rp2040
// library: DHT
// description: Declare a state enum at global scope and drive a DHT22 read from it — IDLE waits
//   out the sample interval, MEASURING takes the reading, REPORTING prints it.
//   Put the interval test inside the state that is WAITING, not in front of the whole switch.
//   Gating every transition on one timer makes a 3-state cycle take 3 intervals, so a sketch
//   that claims to sample every 2 s actually samples every 6 s.
//   DHT22 needs ≥2 s between reads and its readHumidity()/readTemperature() BLOCK for ~25 ms
//   while bit-banging the one-wire line — do not put one in a motor control loop.
//   Braces are required around a case body that declares variables.
//   NOT for the Romi 32U4: pin 2 is its I2C SDA line. Move the DHT to a free pin (e.g. 4 or 8).
// prompts: declare enum State global, state machine DHT22, IDLE MEASURING REPORTING, temperature humidity state machine, non-blocking DHT read
// verified: compile-only
// source: kd-generated (id 143)
// not-tags: romi32u4, romi
// exact-include: DHT.h

#include <DHT.h>

#define DHTPIN 2      // Digital pin connected to the DHT sensor (NOT pin 2 on a Romi — that is SDA)
#define DHTTYPE DHT22 // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

// Plain enum (C-style) for broad AVR compatibility
enum State { IDLE, MEASURING, REPORTING };
State state = IDLE;

unsigned long prevMs = 0;
const unsigned long MEASUREMENT_INTERVAL = 2000; // DHT22 minimum sampling period

float humidity    = NAN;
float temperature = NAN;

void setup() {
  Serial.begin(9600);
  dht.begin();
  prevMs = millis();
}

void loop() {
  switch (state) {
    case IDLE:
      // The ONLY state that waits on the clock. The other two run as soon as they are entered.
      if (millis() - prevMs >= MEASUREMENT_INTERVAL) {
        prevMs = millis();
        state = MEASURING;
      }
      break;

    case MEASURING:
      humidity    = dht.readHumidity();
      temperature = dht.readTemperature();
      state = REPORTING;
      break;

    case REPORTING:
      // isnan() is the DHT's failure signal — a disconnected or mistimed sensor returns NaN,
      // not 0. Printing it unchecked reports a plausible 0 °C from a sensor that is not there.
      if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
      } else {
        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.print("%  Temperature: ");
        Serial.print(temperature);
        Serial.println(" C");
      }
      state = IDLE;
      break;
  }
}
