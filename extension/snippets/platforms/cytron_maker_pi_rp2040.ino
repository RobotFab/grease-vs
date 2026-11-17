// snippet: cytron_maker_pi_rp2040
// tags: rp2040, maker-pi, cytron, pico, motor, servo, neopixel, platform, grove
// boards: rp2040
// library: none
// description: Cytron Maker Pi RP2040 starter — motors on GP8/GP9, servo on GP12, NeoPixels on GP18
// prompts: Cytron Maker Pi RP2040, RP2040 robot board, Maker Pi motor servo NeoPixel, Grove RP2040 starter
// exact-include: Servo.h, Adafruit_NeoPixel.h

#include <Servo.h>
#include <Adafruit_NeoPixel.h>

// Motor A: GP8=dir, GP9=PWM (via analogWrite)
const int M1_DIR = 8, M1_PWM = 9;
const int M2_DIR = 10, M2_PWM = 11;

Servo servo1;

Adafruit_NeoPixel pixels(2, 18, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  pinMode(M1_DIR, OUTPUT); pinMode(M1_PWM, OUTPUT);
  servo1.attach(12);
  pixels.begin();
  pixels.setPixelColor(0, pixels.Color(0, 50, 0));  // green
  pixels.show();
}

void loop() {
  // forward
  digitalWrite(M1_DIR, HIGH); analogWrite(M1_PWM, 200);
  servo1.write(90);
  delay(2000);
  // stop
  analogWrite(M1_PWM, 0);
  delay(500);
}
