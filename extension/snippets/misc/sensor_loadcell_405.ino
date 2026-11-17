// snippet: sensor_loadcell_405
// tags: hx711, servo
// boards: avr, megaavr, nano33iot, rp2040
// not-tags: romi32u4
// library: HX711, Servo
// description: Trigger a servo on pin 9 to open a gate (90°) when HX711 (DOUT=3, CLK=4) weight exceeds 250 g, an…
// prompts: trigger, servo, (90°), hx711, (dout=3, clk=4)
// verified: compile-only
// source: kd-generated (id 405)
// exact-include: HX711.h, Servo.h

#include <HX711.h>
#include <Servo.h>

#define DOUT_PIN 3
#define CLK_PIN  4
#define SERVO_PIN 9

Servo gateServo;
HX711 scale;
unsigned long prevMs = 0;
const unsigned long CHECK_INTERVAL = 100;  // Check weight every 100ms

void setup() {
  Serial.begin(9600);
  gateServo.attach(SERVO_PIN);
  gateServo.write(0);  // Start with gate closed
  scale.begin(DOUT_PIN, CLK_PIN);
  scale.set_scale(2280.0);  // Calibrate with known weight
  scale.tare();  // HX711: zero the scale (averaged reading -> offset)
  Serial.println("Scale ready — place weight");
}

void loop() {
  unsigned long now = millis();
  if (now - prevMs >= CHECK_INTERVAL) {
    prevMs = now;

    if (scale.is_ready()) {
      float grams = scale.get_units(5);  // HX711: averaged reading in units (after tare + set_scale)
      Serial.print("weight: ");
      Serial.println(grams);

      if (grams > 250) {
        gateServo.write(90);  // Open gate
      } else if (grams < 50) {
        gateServo.write(0);  // Close gate
      }
    }
  }
}
